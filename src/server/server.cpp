/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "server.hpp"

#include "message/input.hpp"

namespace df {
  std::auto_ptr< server > server::instance_;

  server::server(std::string const& publisher_address,
                 std::string const& subscriber_address,
                 std::string const& control_address,
                 std::string const& server_uuid,
                 std::string const& password_hash,
                 boost::posix_time::time_duration client_timeout) :
      zmq_context_(1), zmq_publisher_(zmq_context_, ZMQ_PUB), zmq_subscriber_(zmq_context_, ZMQ_SUB), publisher_address_(publisher_address), subscriber_address_(subscriber_address), control_address_(control_address), server_uuid_(server_uuid), password_hash_(password_hash), client_timeout_(client_timeout) {
    std::clog << "server::server(" << publisher_address << "," << subscriber_address << ")\n";

    zmq_publisher_.bind(publisher_address.c_str());

    uint64_t publisher_hwm = 2048;
    zmq_publisher_.setsockopt(ZMQ_HWM, &publisher_hwm, sizeof(publisher_hwm));

    zmq_subscriber_.bind(subscriber_address.c_str());
    df::subscribe(zmq_subscriber_, df::message::input_t());
  }

  server::~server() {
    std::clog << "server::~server()\n";
  }
} // namespace df
