/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_CLIENT_INPUT_HPP_
#define DWARF_FORTRESS_CLIENT_INPUT_HPP_

#include <cstdint>
#include <string>

#include <zmq.hpp>

#include "client.hpp"

namespace df {

  struct input_client {

      df::client_configuration_t const config_;
      input_client(df::client_configuration_t const& config);

      void input(zmq::context_t& zmq_context);

      struct control_thread {
          control_thread(input_client& client, zmq::context_t& zmq_context);

          input_client& client_;
          zmq::context_t& zmq_context_;
          void operator()();
      };

  };

} // namespace df

#endif /* DWARF_FORTRESS_CLIENT_INPUT_HPP_ */
