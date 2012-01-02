/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_SERVER_RENDERER_HPP_
#define DWARF_FORTRESS_SERVER_RENDERER_HPP_

#include <iostream>

#include "g_src/enabler.h"

#include "message.hpp"
#include "message/display.hpp"

#include "server.hpp"

#include "hilbert.hpp"

namespace df {
  class server_renderer: virtual public renderer {
    public:
      df::message::display_t message;

      virtual ~server_renderer() {
      }

      void send_update() {
        if (message.width && message.height) {
          for (std::uint32_t x = message.x; x < message.x + message.width; ++x) {
            graphic_character_t const* characters = reinterpret_cast< graphic_character_t const* >(gps.screen)
                + x * gps.dimy + message.y;
            message.characters.insert(message.characters.end(), characters, characters + message.height);

            texture_index_t const* texture_indexes = reinterpret_cast< texture_index_t const* >(gps.screentexpos)
                + x * gps.dimy + message.y;
            message.texture_indexes.insert(message.texture_indexes.end(),
                                           texture_indexes,
                                           texture_indexes + message.height);

            for (std::uint32_t y = message.y; y < message.y + message.height; ++y) {
              std::uint32_t t = x * gps.dimy + y;
              message.texture_colors.push_back((gps.screentexpos_addcolor[t] << 24)
                  | (gps.screentexpos_grayscale[t] << 16) | (gps.screentexpos_cf[t] << 8) | (gps.screentexpos_cbr[t]));
            }
          }

          df::server::instance().send(message);
          message = df::message::display_t();
        }
      }

      virtual void update_tile(int x, int y) {
        if (message.width && message.height) {
          if ((x >= message.x && x < message.x + message.width) && (y >= message.y && y < message.y + message.height)) {
            return;
          }

          std::uint64_t const hilbert_allowed_margin = 128;
          std::uint64_t const mean_hilbert = df::to_hilbert(message.x + message.width / 2,
                                                            message.y + message.height / 2);
          std::uint64_t const hilbert = df::to_hilbert(x, y);

          if (std::max(mean_hilbert, hilbert) - std::min(mean_hilbert, hilbert) <= hilbert_allowed_margin) {
            std::int32_t const xdiff = x - message.x;
            std::int32_t const ydiff = y - message.y;

            if (xdiff < 0) {
              message.x += xdiff;
              message.width -= xdiff;
            } else if (xdiff >= message.width) {
              message.width = xdiff + 1;
            }

            if (ydiff < 0) {
              message.y += ydiff;
              message.height -= ydiff;
            } else if (ydiff >= message.height) {
              message.height = ydiff + 1;
            }

            return;
          } else {
            send_update();
          }
        }

        message.x = x;
        message.y = y;
        message.width = 1;
        message.height = 1;
      }

      virtual void update_all() {
        message.x = 0;
        message.y = 0;
        message.width = gps.dimx;
        message.height = gps.dimy;

        send_update();
      }
      virtual void render() {
        // Flush the pending update if required.
        send_update();
      }
      virtual void set_fullscreen() {
        // Fullscreen is meaningless on server side.
      }
      virtual void zoom(zoom_commands cmd) {
        std::cerr << "server_renderer::zoom not implemented.\n";
      }
      virtual void resize(int w, int h) {
        std::cerr << "server_renderer::resize(" << w << "," << h << ") not implemented.\n";
        gps_allocate(w, h);

        df::message::display_t reset_message = df::message::display_t();
        reset_message.width = w;
        reset_message.height = h;
        reset_message.resets_bounds = true;

        if (reset_message.x != message.x || reset_message.y != message.y || reset_message.width != message.width
            || reset_message.height != message.height) {
          send_update();
        }

        message = reset_message;
      }
      virtual void grid_resize(int w, int h) {
        std::cerr << "server_renderer::grid_resize(" << w << "," << h << ") not implemented.\n";
        resize(w, h);
      }
      virtual bool get_mouse_coords(int &x, int &y) {
        // TODO: Find a way of retrieving mouse coords from clients?
        std::cerr << "server_renderer::get_mouse_coords not implemented.\n";
        return false;
      }

      virtual bool uses_opengl() {
        return false;
      }
  }
  ;
} // namespace df

#endif /* DWARF_FORTRESS_SERVER_RENDERER_HPP_ */
