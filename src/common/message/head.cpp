/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "message.hpp"
#include "message/head.hpp"

namespace df {

  namespace message {

    head::type const get_message_type< input_t >::value = head::type::input;
    head::type const get_message_type< display_t >::value = head::type::display;
    head::type const get_message_type< session_t >::value = head::type::session;
    head::type const get_message_type< internal_t >::value = head::type::internal;

  } // namespace message

  void packer< message::head_t >::pack(df::bytes& bytes, message::head_t const& value) {
    /* We have to pack bytes in reverse order so we can unpack in order */
    df::pack(bytes, value.type);
    df::pack(bytes, value.peer_uuid);
  }

  void packer< message::head_t >::unpack(df::bytes& bytes, message::head_t& value) {
    df::unpack(bytes, value.peer_uuid);
    df::unpack(bytes, value.type);
  }

} // namespace df

