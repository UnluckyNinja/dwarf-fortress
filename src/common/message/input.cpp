/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "message.hpp"
#include "message/input.hpp"

namespace df {

  void packer< message::input_t >::pack(df::bytes& bytes, message::input_t const& value) {
    std::size_t const expected_size = value.count * sizeof(std::uint64_t) + 1;
    bytes.reserve(bytes.size() + expected_size);

    /* We have to pack bytes in reverse order so we can unpack in order */
    std::for_each(value.keys.rbegin(), value.keys.rend(), [&bytes](std::uint64_t const e) {df::pack(bytes, e);});

    df::pack(bytes, value.count);
  }

  void packer< message::input_t >::unpack(df::bytes& bytes, message::input_t& value) {
    df::unpack(bytes, value.count);

    value.keys.resize(value.count);

    std::for_each(value.keys.begin(), value.keys.end(), [&bytes](std::uint64_t& e) {df::unpack(bytes, e);});
  }

} // namespace df

