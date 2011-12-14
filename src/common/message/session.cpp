/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "message.hpp"
#include "message/session.hpp"

namespace df {

  void packer< message::session_t >::pack(df::bytes& bytes, message::session_t const& value) {
    switch (value.type) {
      case message::session::heartbeat_request:
        break;
      case message::session::heartbeat_response:
        df::pack(bytes, value.connection_status);
        break;
      case message::session::connection_request:
        df::pack(bytes, value.password_hash);
        break;
      case message::session::connection_response:
        df::pack(bytes, value.connection_status);
        break;
    }

    df::pack(bytes, value.type);
  }

  void packer< message::session_t >::unpack(df::bytes& bytes, message::session_t& value) {
    df::unpack(bytes, value.type);

    switch (value.type) {
      case message::session::heartbeat_request:
        break;
      case message::session::heartbeat_response:
        df::unpack(bytes, value.connection_status);
        break;
      case message::session::connection_request:
        df::unpack(bytes, value.password_hash);
        break;
      case message::session::connection_response:
        df::unpack(bytes, value.connection_status);
        break;
    }
  }

} // namespace df
