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
    switch (value.type_) {
      case message::internal::window_resize_response:
        df::pack(bytes, value.height);
        df::pack(bytes, value.width);
        break;
      default:
        break;
    }
    df::pack(bytes, value.type_);
  }

  void packer< message::internal_t >::unpack(df::bytes& bytes, message::internal_t& value) {
    df::unpack(bytes, value.type_);

    switch (value.type_) {
      case message::internal::window_resize_response:
        df::unpack(bytes, value.width);
        df::unpack(bytes, value.height);
        break;
      default:
        break;
    }
  }

} // namespace df
