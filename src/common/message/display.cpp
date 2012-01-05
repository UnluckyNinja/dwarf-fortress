/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "message.hpp"
#include "message/display.hpp"

namespace df {

  void packer< message::tileset_t >::pack(df::bytes& bytes, message::tileset_t const& value) {
    std::for_each(value.pixels.rbegin(), value.pixels.rend(), [&bytes](std::uint32_t const e) {df::pack(bytes, e);});

    df::pack(bytes, value.tile_size_y_);
    df::pack(bytes, value.tile_size_x_);
    df::pack(bytes, value.tile_count_y_);
    df::pack(bytes, value.tile_count_x_);
    df::pack(bytes, value.tileset_id_);
  }

  void packer< message::tileset_t >::unpack(df::bytes& bytes, message::tileset_t& value) {
    df::unpack(bytes, value.tileset_id_);
    df::unpack(bytes, value.tile_count_x_);
    df::unpack(bytes, value.tile_count_y_);
    df::unpack(bytes, value.tile_size_x_);
    df::unpack(bytes, value.tile_size_y_);

    value.pixels.resize(value.tile_count_x_ * value.tile_count_y_ * value.tile_size_x_ * value.tile_size_y_);

    std::for_each(value.pixels.begin(), value.pixels.end(), [&bytes](std::uint32_t& e) {df::unpack(bytes, e);});
  }

  void packer< message::display_t >::pack(df::bytes& bytes, message::display_t const& value) {
    switch (value.type) {
      case message::display::screen_update:
        /* We have to pack bytes in reverse order so we can unpack in order */
        df::pack(bytes, value.resets_bounds);

        std::for_each(value.texture_colors.rbegin(),
                      value.texture_colors.rend(),
                      [&bytes](df::texture_color_t const e) {df::pack(bytes, e);});
        std::for_each(value.texture_indexes.rbegin(),
                      value.texture_indexes.rend(),
                      [&bytes](df::texture_index_t const e) {df::pack(bytes, e);});
        std::for_each(value.characters.rbegin(),
                      value.characters.rend(),
                      [&bytes](df::graphic_character_t const e) {df::pack(bytes, e);});

        df::pack(bytes, value.height);
        df::pack(bytes, value.width);
        df::pack(bytes, value.y);
        df::pack(bytes, value.x);
        break;

      case message::display::tileset_info:
        std::for_each(value.tilesets.rbegin(),
                      value.tilesets.rend(),
                      [&bytes](df::message::tileset_t const e) {df::pack(bytes, e);});

        df::pack(bytes, value.tileset_count);
        break;
    }

    df::pack(bytes, value.type);
  }

  void packer< message::display_t >::unpack(df::bytes& bytes, message::display_t& value) {
    df::unpack(bytes, value.type);

    switch (value.type) {
      case message::display::screen_update:
        df::unpack(bytes, value.x);
        df::unpack(bytes, value.y);
        df::unpack(bytes, value.width);
        df::unpack(bytes, value.height);

        value.characters.resize(value.width * value.height);
        value.texture_indexes.resize(value.width * value.height);
        value.texture_colors.resize(value.width * value.height);

        std::for_each(value.characters.begin(),
                      value.characters.end(),
                      [&bytes](df::graphic_character_t& e) {df::unpack(bytes, e);});
        std::for_each(value.texture_indexes.begin(),
                      value.texture_indexes.end(),
                      [&bytes](df::texture_index_t& e) {df::unpack(bytes, e);});
        std::for_each(value.texture_colors.begin(),
                      value.texture_colors.end(),
                      [&bytes](df::texture_color_t& e) {df::unpack(bytes, e);});

        df::unpack(bytes, value.resets_bounds);
        break;

      case message::display::tileset_info:
        df::unpack(bytes, value.tileset_count);

        value.tilesets.resize(value.tileset_count);

        std::for_each(value.tilesets.begin(),
                      value.tilesets.end(),
                      [&bytes](df::message::tileset_t& e) {df::unpack(bytes, e);});
        break;
    }
  }

} // namespace df
