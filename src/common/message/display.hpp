/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_MESSAGE_DISPLAY_HPP_
#define DWARF_FORTRESS_MESSAGE_DISPLAY_HPP_

#include <cstdint>
#include <vector>

#include "packer.hpp"
#include "varint.hpp"

#include "message/head.hpp"

namespace df {
  typedef std::uint32_t graphic_character_t;
  typedef std::uint32_t texture_index_t;
  typedef std::uint32_t texture_color_t;

  namespace message {

    namespace display {

      enum type: std::uint8_t {
        screen_update,
        tileset_info,
      };

    } // namespace display

    struct tileset_t {
        std::uint16_t tileset_id_;
        std::uint8_t tile_count_x_;
        std::uint8_t tile_count_y_;
        std::uint32_t tile_size_x_;
        std::uint32_t tile_size_y_;
        std::vector< std::uint32_t > pixels;
    };

    struct display_t {
        display::type type;

        std::uint32_t x;
        std::uint32_t y;
        std::uint32_t width;
        std::uint32_t height;
        std::vector< graphic_character_t > characters;
        std::vector< texture_index_t > texture_indexes;
        std::vector< texture_color_t > texture_colors;
        std::uint8_t resets_bounds;

        std::uint32_t tileset_count;
        std::vector< tileset_t > tilesets;
    };

  } // namespace message

  template< >
  struct packer< message::tileset_t > {
      static void pack(df::bytes& bytes, message::tileset_t const& value);
      static void unpack(df::bytes& bytes, message::tileset_t& value);
  };

  template< >
  struct packer< message::display_t > {
      static void pack(df::bytes& bytes, message::display_t const& value);
      static void unpack(df::bytes& bytes, message::display_t& value);
  };

} // namespace df

#endif /* DWARF_FORTRESS_MESSAGE_DISPLAY_HPP_ */
