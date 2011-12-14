/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_INTERNAL_HPP_
#define DWARF_FORTRESS_MESSAGE_INTERNAL_HPP_

#include <cstdint>

#include "packer.hpp"
#include "varint.hpp"

#include "message/head.hpp"

namespace df {
  namespace message {

    namespace internal {

      enum type: std::uint8_t {
        synchronization_request,
        synchronization_response,
      };

    } // namespace internal

    struct internal_t {
        internal::type type_;

        internal_t(internal::type const type) :
            type_(type) {
        }
    };

  } // namespace message

  template< >
  struct packer< message::internal_t > {
      static void pack(df::bytes& bytes, message::internal_t const& value);
      static void unpack(df::bytes& bytes, message::internal_t& value);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_INTERNAL_HPP_ */
