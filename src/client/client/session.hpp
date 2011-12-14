/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_CLIENT_SESSION_HPP_
#define DWARF_FORTRESS_CLIENT_SESSION_HPP_

#include <cstdint>
#include <string>

#include <zmq.hpp>

#include "client.hpp"

namespace df {

  struct session_client {
      struct timeout_error: std::exception {
      };
      struct unexpected_response_type_error: std::exception {
      };
      struct connection_refused_error: std::exception {
      };

      enum state {
        waiting_for_server, not_connected, connected
      };

      df::session_client::state session_state_;
      std::string server_uuid_;

      df::client_configuration_t const config_;
      session_client(df::client_configuration_t const& config);

      void session(zmq::context_t& zmq_context);

      struct control_thread {
          control_thread(session_client& client, zmq::context_t& zmq_context);

          session_client& client_;
          zmq::context_t& zmq_context_;
          void operator()();
      };

  };

} // namespace df

#endif /* DWARF_FORTRESS_CLIENT_SESSION_HPP_ */
