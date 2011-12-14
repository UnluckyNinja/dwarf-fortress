/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_INPUT_HPP_
#define DWARF_FORTRESS_MESSAGE_INPUT_HPP_

#include <cstdint>
#include <vector>

#include "packer.hpp"
#include "varint.hpp"

#include "message/head.hpp"

namespace df {
  namespace message {

    struct input_t {
        std::uint64_t count;
        std::vector< std::uint64_t > keys;
    };

  } // namespace message

  template< >
  struct packer< message::input_t > {
      static void pack(df::bytes& bytes, message::input_t const& value);
      static void unpack(df::bytes& bytes, message::input_t& value);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_INPUT_HPP_ */
