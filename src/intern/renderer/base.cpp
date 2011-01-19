/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "integer_types.hpp"

#include "extern/renderer/base.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"
#include "extern/graphics.hpp"

#include "intern/texture/full_id.hpp"
#include "intern/texture/resize.hpp"
#include "intern/ttf_manager.hpp"

#include "intern/utils/numbers.hpp"

#include "meta/either.hpp"

#include <logging/logging.hpp>

SDL_Surface* renderer_2d_base::tile_cache_lookup(texture_fullid &id) {
  ::std::map< texture_fullid, SDL_Surface* >::iterator it = tile_cache.find(id);
  if (it != tile_cache.end()) {
    return it->second;
  } else {
    // Create the colorized texture
    SDL_Surface *tex = enabler.textures.get_texture_data(id.texpos);
    SDL_Surface * color;
    color = SDL_CreateRGBSurface(SDL_SWSURFACE, tex->w, tex->h, tex->format->BitsPerPixel, tex->format->Rmask, tex->format->Gmask, tex->format->Bmask, 0);
    if (!color) {
      __fatal
        << "Unable to create texture!";
      abort();
    }

    // Fill it
    uint32_t color_fgi = SDL_MapRGB(color->format, id.r * 255, id.g * 255, id.b * 255);
    uint8_t *color_fg = (uint8_t*) &color_fgi;
    uint32_t color_bgi = SDL_MapRGB(color->format, id.br * 255, id.bg * 255, id.bb * 255);
    uint8_t *color_bg = (uint8_t*) &color_bgi;
    SDL_LockSurface(tex);
    SDL_LockSurface( color);

    uint8_t *pixel_src, *pixel_dst;
    for (int y = 0; y < tex->h; y++) {
      pixel_src = ((uint8_t*) tex->pixels) + (y * tex->pitch);
      pixel_dst = ((uint8_t*) color->pixels) + (y * color->pitch);
      for (int x = 0; x < tex->w; x++, pixel_src += 4, pixel_dst += 4) {
        float alpha = pixel_src[3] / 255.0;
        for (int c = 0; c < 3; c++) {
          float fg = color_fg[c] / 255.0, bg = color_bg[c] / 255.0, tex = pixel_src[c] / 255.0;
          pixel_dst[c] = ((alpha * (tex * fg)) + ((1 - alpha) * bg)) * 255;
        }
      }
    }

    SDL_UnlockSurface(color);
    SDL_UnlockSurface(tex);

    // Convert to display format; deletes color
    SDL_Surface *disp = SDL_Resize(color, dispx_z, dispy_z);
    // Insert and return
    tile_cache[id] = disp;
    return disp;
  }
}

bool renderer_2d_base::init_video(int w, int h) {
  // Get ourselves a 2D SDL window
  uint32_t flags = init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DHW) ? SDL_HWSURFACE : SDL_SWSURFACE;
  flags |= init.display.flag.has_flag(INIT_DISPLAY_FLAG_2DASYNC) ? SDL_ASYNCBLIT : 0;

  // Set it up for windowed or fullscreen, depending.
  if (enabler.is_fullscreen()) {
    flags |= SDL_FULLSCREEN;
  } else {
    if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE))
      flags |= SDL_RESIZABLE;
  }

  // (Re)create the window
  screen = SDL_SetVideoMode(w, h, 32, flags);
  if (screen == NULL) {
    __info
      << "INIT FAILED!";
  }

  return screen != NULL;
}

void renderer_2d_base::update_tile(int x, int y) {
  // Figure out where to blit
  SDL_Rect dst;
  dst.x = dispx_z * x + origin_x;
  dst.y = dispy_z * y + origin_y;
  // Read tiles from gps, create cached texture
  Either< texture_fullid, texture_ttfid > id = screen_to_texid(x, y);
  SDL_Surface * tex;
  if (id.isL) { // Ordinary tile, cached here
    tex = tile_cache_lookup(id.left);
    // And blit.
    SDL_BlitSurface(tex, NULL, screen, &dst);
  } else { // TTF, cached in ttf_manager so no point in also caching here
    if (id.right) {
      tex = ttf_manager.get_texture(id.right);
      // Blit later
      ttfs_to_render.push_back(::std::make_pair(tex, dst));
    }
  }
}

void renderer_2d_base::update_all() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  for (int x = 0; x < gps.dimx; x++)
    for (int y = 0; y < gps.dimy; y++)
      update_tile(x, y);
}

void renderer_2d_base::render() {
  // Render the TTFs, which we left for last
  for (auto it = ttfs_to_render.begin(); it != ttfs_to_render.end(); ++it)
    SDL_BlitSurface(it->first, NULL, screen, &it->second);
  ttfs_to_render.clear();
  // And flip out.
  SDL_Flip(screen);
}

renderer_2d_base::~renderer_2d_base() {
  for (::std::map< texture_fullid, SDL_Surface* >::iterator it = tile_cache.begin(); it != tile_cache.end(); ++it) {
    SDL_FreeSurface(it->second);
  }
}

void renderer_2d_base::grid_resize(int w, int h) {
  dimx = w;
  dimy = h;
  // Only reallocate the grid if it actually changes
  if (init.display.grid_x != dimx || init.display.grid_y != dimy)
    gps_allocate(dimx, dimy);
  // But always force a full display cycle
  gps.force_full_display_count = 1;
  enabler.flag |= ENABLERFLAG_RENDER;
}

renderer_2d_base::renderer_2d_base() {
  zoom_steps = forced_steps = 0;
}

void renderer_2d_base::compute_forced_zoom() {
  forced_steps = 0;
  ::std::pair< int, int > zoomed = compute_zoom();
  while (zoomed.first < MIN_GRID_X || zoomed.second < MIN_GRID_Y) {
    forced_steps++;
    zoomed = compute_zoom();
  }
  while (zoomed.first > MAX_GRID_X || zoomed.second > MAX_GRID_Y) {
    forced_steps--;
    zoomed = compute_zoom();
  }
}

::std::pair< int, int > renderer_2d_base::compute_zoom(bool clamp) {
  const int dispx = enabler.is_fullscreen() ? init.font.large_font_dispx : init.font.small_font_dispx;
  const int dispy = enabler.is_fullscreen() ? init.font.large_font_dispy : init.font.small_font_dispy;
  int w, h;
  if (dispx < dispy) {
    w = natural_w + zoom_steps + forced_steps;
    h = double(natural_h) * (double(w) / double(natural_w));
  } else {
    h = natural_h + zoom_steps + forced_steps;
    w = double(natural_w) * (double(h) / double(natural_h));
  }
  if (clamp) {
    w = ::numbers::clamp(w, MIN_GRID_X, MAX_GRID_X);
    h = ::numbers::clamp(h, MIN_GRID_Y, MAX_GRID_Y);
  }
  return ::std::make_pair(w, h);
}

void renderer_2d_base::resize(int w, int h) {
  // We've gotten resized.. first step is to reinitialize video
  __info
    << "New window size: " << w << "x" << h;
  init_video(w, h);
  dispx = enabler.is_fullscreen() ? init.font.large_font_dispx : init.font.small_font_dispx;
  dispy = enabler.is_fullscreen() ? init.font.large_font_dispy : init.font.small_font_dispy;
  __info
    << "Font size: " << dispx << "x" << dispy;
  // If grid size is currently overridden, we don't change it
  if (enabler.overridden_grid_sizes.size() == 0) {
    // (Re)calculate grid-size
    dimx = ::numbers::clamp(w / dispx, MIN_GRID_X, MAX_GRID_X);
    dimy = ::numbers::clamp(h / dispy, MIN_GRID_Y, MAX_GRID_Y);
    __info
      << "Resizing grid to " << dimx << "x" << dimy;
    grid_resize(dimx, dimy);
  }
  // Calculate zoomed tile size
  natural_w = ::std::max(w / dispx, 1);
  natural_h = ::std::max(h / dispy, 1);
  compute_forced_zoom();
  reshape(compute_zoom(true));
}

void renderer_2d_base::reshape(::std::pair< int, int > max_grid) {
  int w = max_grid.first, h = max_grid.second;
  // Compute the largest tile size that will fit this grid into the window, roughly maintaining aspect ratio
  double try_x = dispx, try_y = dispy;
  try_x = screen->w / w;
  try_y = ::std::min(try_x / dispx * dispy, double(screen->h) / h);
  try_x = ::std::min(try_x, try_y / dispy * dispx);
  dispx_z = ::std::max(1, static_cast< int > (try_x));
  dispy_z = ::std::max(static_cast< int > (try_y), 1);
  __info
    << "Resizing font to " << dispx_z << "x" << dispy_z;
  // Remove now-obsolete tile catalog
  for (::std::map< texture_fullid, SDL_Surface* >::iterator it = tile_cache.begin(); it != tile_cache.end(); ++it)
    SDL_FreeSurface(it->second);
  tile_cache.clear();
  // Recompute grid based on the new tile size
  w = ::numbers::clamp(screen->w / dispx_z, MIN_GRID_X, MAX_GRID_X);
  h = ::numbers::clamp(screen->h / dispy_z, MIN_GRID_Y, MAX_GRID_Y);
  // Reset grid size
#ifdef DEBUG
  __info
  << "Resizing grid to " << w << "x" << h;
#endif
  gps_allocate(w, h);
  // Force redisplay
  gps.force_full_display_count = 1;
  // Calculate viewport origin, for centering
  origin_x = (screen->w - dispx_z * w) / 2;
  origin_y = (screen->h - dispy_z * h) / 2;
  // Reset TTF rendering
  ttf_manager.init(dispy_z, dispx_z);
}

void renderer_2d_base::set_fullscreen() {
  if (enabler.is_fullscreen()) {
    init.display.desired_windowed_width = screen->w;
    init.display.desired_windowed_height = screen->h;
    resize(init.display.desired_fullscreen_width, init.display.desired_fullscreen_height);
  } else {
    resize(init.display.desired_windowed_width, init.display.desired_windowed_height);
  }
}

bool renderer_2d_base::get_mouse_coords(int &x, int &y) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  mouse_x -= origin_x;
  mouse_y -= origin_y;
  if (mouse_x < 0 || mouse_x >= dispx_z * dimx || mouse_y < 0 || mouse_y >= dispy_z * dimy)
    return false;
  x = mouse_x / dispx_z;
  y = mouse_y / dispy_z;
  return true;
}

void renderer_2d_base::zoom(zoom_commands cmd) {
  ::std::pair< int, int > before = compute_zoom(true);
  int before_steps = zoom_steps;
  switch (cmd) {
    case zoom_in:
      zoom_steps -= init.input.zoom_speed;
      break;
    case zoom_out:
      zoom_steps += init.input.zoom_speed;
      break;
    case zoom_reset:
      zoom_steps = 0;
    case zoom_resetgrid:
      compute_forced_zoom();
      break;
    default:
      break;
  }
  ::std::pair< int, int > after = compute_zoom(true);
  if (after == before && (cmd == zoom_in || cmd == zoom_out))
    zoom_steps = before_steps;
  else
    reshape(after);
}

