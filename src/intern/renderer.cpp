/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/renderer.hpp"

#include "hidden/global.hpp"
#include "extern/graphics.hpp"
#include "extern/init.hpp"

#include "intern/texture/full_id.hpp"
#include "intern/config.hpp"

#include <cassert>

void renderer::gps_allocate(int x, int y) {
  if (screen)
    delete[] screen;
  if (screentexpos)
    delete[] screentexpos;
  if (screentexpos_addcolor)
    delete[] screentexpos_addcolor;
  if (screentexpos_grayscale)
    delete[] screentexpos_grayscale;
  if (screentexpos_cf)
    delete[] screentexpos_cf;
  if (screentexpos_cbr)
    delete[] screentexpos_cbr;
  if (screen_old)
    delete[] screen_old;
  if (screentexpos_old)
    delete[] screentexpos_old;
  if (screentexpos_addcolor_old)
    delete[] screentexpos_addcolor_old;
  if (screentexpos_grayscale_old)
    delete[] screentexpos_grayscale_old;
  if (screentexpos_cf_old)
    delete[] screentexpos_cf_old;
  if (screentexpos_cbr_old)
    delete[] screentexpos_cbr_old;

  gps.screen = screen = new unsigned char[x * y * 4];
  memset(screen, 0, x * y * 4);
  gps.screentexpos = screentexpos = new long[x * y];
  memset(screentexpos, 0, x * y * sizeof(long));
  gps.screentexpos_addcolor = screentexpos_addcolor = new char[x * y];
  memset(screentexpos_addcolor, 0, x * y);
  gps.screentexpos_grayscale = screentexpos_grayscale = new unsigned char[x * y];
  memset(screentexpos_grayscale, 0, x * y);
  gps.screentexpos_cf = screentexpos_cf = new unsigned char[x * y];
  memset(screentexpos_cf, 0, x * y);
  gps.screentexpos_cbr = screentexpos_cbr = new unsigned char[x * y];
  memset(screentexpos_cbr, 0, x * y);

  screen_old = new unsigned char[x * y * 4];
  memset(screen_old, 0, x * y * 4);
  screentexpos_old = new long[x * y];
  memset(screentexpos_old, 0, x * y * sizeof(long));
  screentexpos_addcolor_old = new char[x * y];
  memset(screentexpos_addcolor_old, 0, x * y);
  screentexpos_grayscale_old = new unsigned char[x * y];
  memset(screentexpos_grayscale_old, 0, x * y);
  screentexpos_cf_old = new unsigned char[x * y];
  memset(screentexpos_cf_old, 0, x * y);
  screentexpos_cbr_old = new unsigned char[x * y];
  memset(screentexpos_cbr_old, 0, x * y);

  gps.resize(x, y);
}

Either< texture_fullid, texture_ttfid > renderer::screen_to_texid(int x, int y) {
  config const& conf = config::instance();

  const int tile = x * gps.dimy + y;

  const unsigned char *s = screen + tile * 4;

  // TTF text does not get the full treatment.
  if (s[3] == GRAPHICSTYPE_TTF) {
    texture_ttfid texpos = (s[0] << 16) | (s[1] << 8) | s[2];
    return Either< texture_fullid, texture_ttfid > (texpos);
  } else if (s[3] == GRAPHICSTYPE_TTFCONT)
    return Either< texture_fullid, texture_ttfid > (0);

  // Otherwise, it's a normal (graphical?) tile.
  struct texture_fullid ret;
  const int ch = s[0];
  const int bold = (s[3] != 0) * 8;
  const int fg = (s[1] + bold) % 16;
  const int bg = s[2] % 16;

  static bool use_graphics = conf.texture().use_graphics;

  if (use_graphics) {
    const long texpos = screentexpos[tile];
    const char addcolor = screentexpos_addcolor[tile];
    const unsigned char grayscale = screentexpos_grayscale[tile];
    const unsigned char cf = screentexpos_cf[tile];
    const unsigned char cbr = screentexpos_cbr[tile];

    if (texpos) {
      ret.texpos = texpos;
      if (grayscale) {
        ret.r = enabler.ccolor[cf][0];
        ret.g = enabler.ccolor[cf][1];
        ret.b = enabler.ccolor[cf][2];
        ret.br = enabler.ccolor[cbr][0];
        ret.bg = enabler.ccolor[cbr][1];
        ret.bb = enabler.ccolor[cbr][2];
      } else if (addcolor) {
        goto use_ch;
      } else {
        ret.r = ret.g = ret.b = 1;
        ret.br = ret.bg = ret.bb = 0;
      }
      goto skip_ch;
    }
  }

  ret.texpos = enabler.is_fullscreen() ? init.font.large_font_texpos[ch] : init.font.small_font_texpos[ch];
  use_ch: ret.r = enabler.ccolor[fg][0];
  ret.g = enabler.ccolor[fg][1];
  ret.b = enabler.ccolor[fg][2];
  ret.br = enabler.ccolor[bg][0];
  ret.bg = enabler.ccolor[bg][1];
  ret.bb = enabler.ccolor[bg][2];

  skip_ch:

  return Either< texture_fullid, texture_ttfid > (ret);
}

void renderer::display() {
  config const& conf = config::instance();

  const int dimx = init.display.grid_x;
  const int dimy = init.display.grid_y;
  static bool use_graphics = conf.texture().use_graphics;
  if (gps.force_full_display_count) {
    // Update the entire screen
    update_all();
  } else {
    uint32_t *screenp = (uint32_t*) screen, *oldp = (uint32_t*) screen_old;
    if (use_graphics) {
      int off = 0;
      for (int x2 = 0; x2 < dimx; x2++) {
        for (int y2 = 0; y2 < dimy; y2++, ++off, ++screenp, ++oldp) {
          // We don't use pointers for the non-screen arrays because we mostly fail at the
          // *first* comparison, and having pointers for the others would exceed register
          // count.
          // Partial printing (and color-conversion): Big-ass if.
          if (*screenp == *oldp && screentexpos[off] == screentexpos_old[off] && screentexpos_addcolor[off] == screentexpos_addcolor_old[off] && screentexpos_grayscale[off]
              == screentexpos_grayscale_old[off] && screentexpos_cf[off] == screentexpos_cf_old[off] && screentexpos_cbr[off] == screentexpos_cbr_old[off]) {
            // Nothing's changed, this clause deliberately empty
          } else {
            update_tile(x2, y2);
          }
        }
      }
    } else {
      for (int x2 = 0; x2 < dimx; ++x2) {
        for (int y2 = 0; y2 < dimy; ++y2, ++screenp, ++oldp) {
          if (*screenp != *oldp) {
            update_tile(x2, y2);
          }
        }
      }
    }
  }
  if (gps.force_full_display_count > 0)
    gps.force_full_display_count--;
}

void renderer::set_fullscreen() {
} // Should read from enabler.is_fullscreen()

void renderer::zoom(zoom_commands cmd) {
}

void renderer::swap_arrays() {
  screen = screen_old;
  screen_old = gps.screen;
  gps.screen = screen;
  screentexpos = screentexpos_old;
  screentexpos_old = gps.screentexpos;
  gps.screentexpos = screentexpos;
  screentexpos_addcolor = screentexpos_addcolor_old;
  screentexpos_addcolor_old = gps.screentexpos_addcolor;
  gps.screentexpos_addcolor = screentexpos_addcolor;
  screentexpos_grayscale = screentexpos_grayscale_old;
  screentexpos_grayscale_old = gps.screentexpos_grayscale;
  gps.screentexpos_grayscale = screentexpos_grayscale;
  screentexpos_cf = screentexpos_cf_old;
  screentexpos_cf_old = gps.screentexpos_cf;
  gps.screentexpos_cf = screentexpos_cf;
  screentexpos_cbr = screentexpos_cbr_old;
  screentexpos_cbr_old = gps.screentexpos_cbr;
  gps.screentexpos_cbr = screentexpos_cbr;

  gps.screen_limit = gps.screen + gps.dimx * gps.dimy * 4;
}

renderer::renderer() {
  screen = NULL;
  screentexpos = NULL;
  screentexpos_addcolor = NULL;
  screentexpos_grayscale = NULL;
  screentexpos_cf = NULL;
  screentexpos_cbr = NULL;
  screen_old = NULL;
  screentexpos_old = NULL;
  screentexpos_addcolor_old = NULL;
  screentexpos_grayscale_old = NULL;
  screentexpos_cf_old = NULL;
  screentexpos_cbr_old = NULL;
}

bool renderer::uses_opengl() {
  return false;
}
