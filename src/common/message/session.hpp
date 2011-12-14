/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_SESSION_HPP_
#define DWARF_FORTRESS_MESSAGE_SESSION_HPP_

#include <cstdint>

#include "packer.hpp"
#include "varint.hpp"

#include "message/head.hpp"

namespace df {
  namespace message {

    namespace session {

      enum type: std::uint8_t {
        heartbeat_request,
        heartbeat_response,
        connection_request,
        connection_response,
        tileset_request,
        tileset_response
      };

      enum connection_status: std::uint8_t {
        disconnected,
        connected
      };

    } // namespace session

    struct session_t {
        session::type type;

        std::string password_hash;
        session::connection_status connection_status;
    };

  } // namespace message

  template< >
  struct packer< message::session_t > {
      static void pack(df::bytes& bytes, message::session_t const& value);
      static void unpack(df::bytes& bytes, message::session_t& value);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_SESSION_HPP_ */
