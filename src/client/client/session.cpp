/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "session.hpp"

#include <boost/thread.hpp>

#include "signals.hpp"
#include "message.hpp"
#include "message/session.hpp"
#include "message/internal.hpp"

namespace df {

  session_client::session_client(df::client_configuration_t const& config) :
      session_state_(waiting_for_server), server_uuid_(""), config_(config) {
  }

  void session_client::session(zmq::context_t& zmq_context) {
    namespace session = df::message::session;

    zmq::socket_t zmq_requester(zmq_context, ZMQ_REQ);
    std::string const zmq_address = config_.connection_protocol + "://" + config_.server_name + ":"
        + config_.session_port;
    zmq_requester.connect(zmq_address.c_str());

    df::message::session_t response;
    df::message::session_t request;
    request.password_hash = config_.password_hash;

    session_state_ = waiting_for_server;
    boost::system_time request_time = boost::posix_time::not_a_date_time;
    while (!df::kill_received()) {
      if (request_time != boost::posix_time::not_a_date_time) {

        /* Check a few times for a response, and throw a timeout error if none was received
         */
        std::string peer_uuid;
        while (!df::receive(zmq_requester, peer_uuid, response)) {
          boost::system_time current_time = boost::posix_time::second_clock::universal_time();

          if (current_time - request_time > config_.timeout) {
            throw timeout_error();
          }

          boost::this_thread::sleep(config_.timeout / config_.retries_count);
        }

        switch (session_state_) {
          case waiting_for_server:
            if (response.type != session::type::heartbeat_response) {
              throw unexpected_response_type_error();
            }
            break;
          case not_connected:
            if (response.type != session::type::connection_response) {
              throw unexpected_response_type_error();
            }
            break;
          case connected:
            if (response.type != session::type::heartbeat_response) {
              throw unexpected_response_type_error();
            }
            break;
        }

        switch (response.connection_status) {
          case session::connection_status::connected:
            session_state_ = connected;
            server_uuid_ = peer_uuid;
            boost::this_thread::sleep(config_.timeout);
            break;

          case session::connection_status::disconnected:
            if (session_state_ == not_connected) {
              throw connection_refused_error();
            } else {
              std::clog << "W: client disconnected, reconnecting...\n";
              session_state_ = not_connected;
              server_uuid_ = peer_uuid;
            }
            break;
        }
        request_time = boost::posix_time::not_a_date_time;

      } else {

        switch (session_state_) {
          case connected:
          case waiting_for_server:
            request.type = session::type::heartbeat_request;
            df::send(zmq_requester, config_.client_uuid, request);
            request_time = boost::posix_time::second_clock::universal_time();
            break;

          case not_connected:
            request.type = session::type::connection_request;
            df::send(zmq_requester, config_.client_uuid, request);
            request_time = boost::posix_time::second_clock::universal_time();
            break;

        }

      }
    }
  }

  session_client::control_thread::control_thread(session_client& client, zmq::context_t& zmq_context) :
      client_(client), zmq_context_(zmq_context) {
  }

  void session_client::control_thread::operator()() {
    /* Wait for display thread to listen on display control */
    zmq::socket_t zmq_display(zmq_context_, ZMQ_REQ);
    zmq_display.bind("inproc://display");
    {
      df::bytes request;
      df::pack(request, df::message::internal_t(df::message::internal::synchronization_request));
      df::send(zmq_display, request, true);

      df::bytes response;
      df::receive(zmq_display, response, true);
    }

    while (!df::kill_received()) {
      try {
        client_.session(zmq_context_);
      } catch (df::session_client::timeout_error&) {
        std::clog << "W: client timed out, reconnecting...\n";
      } catch (df::session_client::unexpected_response_type_error&) {
        std::clog << "E: client received unexpected response, is the server fine?\n";
      } catch (df::session_client::connection_refused_error&) {
        std::clog << "E: client connection refused, check your password.\n";
        break;
      }
    }
  }

} // namespace df
