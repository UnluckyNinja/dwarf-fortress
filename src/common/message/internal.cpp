/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "message.hpp"
#include "message/internal.hpp"

namespace df {

  void packer< message::internal_t >::pack(df::bytes& bytes, message::internal_t const& value) {
    df::pack(bytes, value.type_);
  }

  void packer< message::internal_t >::unpack(df::bytes& bytes, message::internal_t& value) {
    df::unpack(bytes, value.type_);
  }

} // namespace df
