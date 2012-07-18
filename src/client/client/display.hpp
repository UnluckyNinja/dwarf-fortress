/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_CLIENT_DISPLAY_HPP_
#define DWARF_FORTRESS_CLIENT_DISPLAY_HPP_

#include <cstdint>
#include <string>

#include <zmq.hpp>

#include "client.hpp"

namespace df {

  struct display_client {

      df::client_configuration_t const config_;
      display_client(df::client_configuration_t const& config);

      void display(zmq::context_t& zmq_context);

      struct control_thread {
          control_thread(display_client& client, zmq::context_t& zmq_context);

          display_client& client_;
          zmq::context_t& zmq_context_;
          void operator()();
      };

  };

} // namespace df

#endif /* DWARF_FORTRESS_CLIENT_DISPLAY_HPP_ */
