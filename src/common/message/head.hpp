/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_HEAD_HPP_
#define DWARF_FORTRESS_MESSAGE_HEAD_HPP_

#include <cstdint>
#include <vector>

#include "packer.hpp"
#include "varint.hpp"

namespace df {
  namespace message {

    struct input_t;
    struct display_t;
    struct session_t;
    struct internal_t;

    namespace head {

      enum type: std::uint8_t {
        input = 'I',
        display = 'G',
        session = 'S',
        internal = '-',
        unknown = '?',
      };

    } // namespace head

    template< typename MessageType >
    struct get_message_type {
        static head::type const value = head::type::unknown;
    };
    template< >
    struct get_message_type< input_t > {
        static head::type const value;
    };
    template< >
    struct get_message_type< display_t > {
        static head::type const value;
    };
    template< >
    struct get_message_type< session_t > {
        static head::type const value;
    };
    template< >
    struct get_message_type< internal_t > {
        static head::type const value;
    };

    struct head_t {
        head::type type;
        std::string peer_uuid;
    };

  } // namespace message

  template< >
  struct packer< message::head_t > {
      static void pack(df::bytes& bytes, message::head_t const& value);
      static void unpack(df::bytes& bytes, message::head_t& value);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_HEAD_HPP_ */
