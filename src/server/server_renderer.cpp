/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "server_renderer.hpp"

#include "g_src/init.h"

namespace df {
} // namespace df

void renderer::display() {
  if (gps.force_full_display_count) {
    update_all();

  } else {
    static bool use_graphics = init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS);

    static std::uint32_t dimx = 0xFFFFFF;
    static std::uint32_t dimy = 0xFFFFFF;
    static std::vector< std::uint64_t > hilbert_range;

    if (dimx != init.display.grid_x || dimy != init.display.grid_y) {
      dimx = init.display.grid_x;
      dimy = init.display.grid_y;
      hilbert_range = df::to_hilbert_range(0, 0, dimx, dimy);
    }

    for (std::uint64_t hilbert : hilbert_range) {
      std::pair< std::uint32_t, std::uint32_t > const coord = df::from_hilbert(hilbert);
      std::uint32_t const x = coord.first;
      std::uint32_t const y = coord.second;
      std::size_t const tile = x * gps.dimy + y;

      if (((uint32_t*) screen)[tile] != ((uint32_t*) screen_old)[tile]
          || (use_graphics
              && (screentexpos[tile] != screentexpos_old[tile]
                  || screentexpos_addcolor[tile] != screentexpos_addcolor_old[tile]
                  || screentexpos_grayscale[tile] != screentexpos_grayscale_old[tile]
                  || screentexpos_cf[tile] != screentexpos_cf_old[tile]
                  || screentexpos_cbr[tile] != screentexpos_cbr_old[tile]))) {
        update_tile(x, y);
      }
    }

    render();
  }

  if (gps.force_full_display_count > 0)
    gps.force_full_display_count--;
}
