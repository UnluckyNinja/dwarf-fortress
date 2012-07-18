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

        //    std::clog << "sending graphic update (x: " << value.x << ", y: " << value.y << ", w: " << value.width << ", h: "
        //        << value.height << ")\n";
        //    for (std::size_t ly = 0; ly < value.height; ++ly) {
        //      for (std::size_t lx = 0; lx < x; ++lx) {
        //        std::clog << " ";
        //      }
        //
        //      for (std::size_t lx = 0; lx < value.width; ++lx) {
        //        char c = (char) value.characters[lx * value.height + ly];
        //        if (std::isprint(c)) {
        //          std::clog << c;
        //        } else {
        //          std::clog << ".";
        //        }
        //      }
        //      std::clog << "\n";
        //    }
        break;

      case message::display::tileset_info:
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

        //    std::clog << "received graphic update (x: " << value.x << ", y: " << value.y << ", w: " << value.width << ", h: "
        //        << value.height << ")\n";
        //    for (std::size_t ly = 0; ly < value.height; ++ly) {
        //      for (std::size_t lx = 0; lx < x; ++lx) {
        //        std::clog << " ";
        //      }
        //
        //      for (std::size_t lx = 0; lx < value.width; ++lx) {
        //        char c = (char) value.characters[lx * value.height + ly];
        //        if (std::isprint(c)) {
        //          std::clog << c;
        //        } else {
        //          std::clog << ".";
        //        }
        //      }
        //      std::clog << "\n";
        //    }
        break;

      case message::display::tileset_info:
        break;
    }
  }

} // namespace df
