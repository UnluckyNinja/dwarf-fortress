/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_SERVER_HPP_
#define DWARF_FORTRESS_SERVER_HPP_

#include <iostream>
#include <memory>

#include <zmq.hpp>
#include <boost/thread.hpp>

#include "signals.hpp"
#include "message.hpp"
#include "message/input.hpp"
#include "message/session.hpp"

namespace df {
  class server {
      zmq::context_t zmq_context_;
      zmq::socket_t zmq_publisher_;
      zmq::socket_t zmq_subscriber_;

    public:
      static std::auto_ptr< server > instance_;
      static server& instance() {
        return *instance_;
      }

      server(std::string const& publisher_address,
             std::string const& subscriber_address,
             std::string const& control_address,
             std::string const& server_uuid,
             std::string const& password_hash,
             boost::posix_time::time_duration client_timeout = boost::posix_time::seconds(15));
      virtual ~server();

      bool receive(df::message::input_t& message) {
        std::string peer_uuid;
        return df::receive(zmq_subscriber_, peer_uuid, message);
      }

      template< typename MessageType >
      void send(MessageType const& message) {
        while (!df::send(zmq_publisher_, server_uuid_, message))
          std::clog << "failed to send, retrying...\n";
      }

      std::string publisher_address_;
      std::string subscriber_address_;

      std::string control_address_;
      std::string server_uuid_;
      std::string password_hash_;
      boost::posix_time::time_duration client_timeout_;

      struct client_session {
          std::string client_uuid;
          df::message::session::connection_status status;
          boost::system_time last_request_time;
      };
      typedef std::map< std::string, client_session > client_session_map_t;
      client_session_map_t client_sessions_;

      void control_flow(zmq::context_t& zmq_context) {
        namespace session = df::message::session;

        zmq::socket_t zmq_responder(zmq_context, ZMQ_REP);
        zmq_responder.bind(control_address_.c_str());

        df::message::session_t request;
        df::message::session_t response;
        while (!df::kill_received()) {
          boost::system_time current_time = boost::posix_time::second_clock::universal_time();

          std::string peer_uuid;
          if (df::receive(zmq_responder, peer_uuid, request)) {
            if (client_sessions_.find(peer_uuid) == client_sessions_.end()) {
              client_sessions_[peer_uuid].client_uuid = peer_uuid;
              client_sessions_[peer_uuid].status = session::connection_status::disconnected;
            }
            client_session& session = client_sessions_[peer_uuid];
            session.last_request_time = current_time;

            switch (request.type) {
              case session::type::connection_request:
                if (request.password_hash == password_hash_) {
                  std::clog << "I: new client " << peer_uuid << " accepted.\n";
                  session.status = session::connection_status::connected;
                } else {
                  std::clog << "I: new client " << peer_uuid << " refused.\n";
                  session.status = session::connection_status::disconnected;
                }

                response.type = session::type::connection_response;
                break;

              case session::type::heartbeat_request:
              default:
                response.type = session::type::heartbeat_response;
                break;
            }
            response.connection_status = session.status;

            df::send(zmq_responder, server_uuid_, response);
          }

          /* Find and purge timed-out clients. */
          std::vector< std::string > timed_out_clients;
          std::for_each(client_sessions_.begin(),
                        client_sessions_.end(),
                        [&](client_session_map_t::value_type const& pair) {if(pair.second.last_request_time + client_timeout_ < current_time) {timed_out_clients.push_back(pair.first);};});
          std::for_each(timed_out_clients.begin(),
                        timed_out_clients.end(),
                        [&](std::string const& client_uuid) {client_sessions_.erase(client_uuid);});
          for (std::string const& client_uuid : timed_out_clients) {
            std::clog << "W: client " << client_uuid << " timed out.\n";
          }

          boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        }
      }

      struct control_thread {
          control_thread(server& server) :
              server_(server) {
          }

          server& server_;
          void operator()() {
            zmq::context_t zmq_context(1);

            server_.control_flow(zmq_context);
          }
      };

  };
} // namespace df

#endif /* DWARF_FORTRESS_SERVER_HPP_ */
