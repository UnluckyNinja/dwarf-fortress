/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/enabler.hpp"

#include "extern/textures.hpp"

#include <logging/logging.hpp>

#include <SDL.h>
#include <SDL_image.h>

SDL_Surface *textures::get_texture_data(long pos) {
  if (raws.size() > pos)
    return raws[pos];
  else {
    __error
      << "Asked for nonexistent texture data";
    return NULL;
  }
}

long textures::clone_texture(long src) {
  long tx;

  if (raws.size() > src && raws[src]) {
    SDL_Surface *dst = SDL_ConvertSurface(raws[src], raws[src]->format, SDL_SWSURFACE);
    tx = add_texture(dst);
  } else {
    // Okay, we've been asked to clone a nonexistent texture. Riight...
    // Fortunately add_texture doesn't look at the pointer it gets at all.
    __error
      << "Asked to clone nonexistent texture!";
    tx = add_texture(NULL);
  }

  enabler.reset_textures();

  return tx;
}

void textures::load_multi_pdim(const ::std::string &filename, long *tex_pos, long dimx, long dimy,
    bool convert_magenta, long *disp_x, long *disp_y) {
  SDL_Surface *raw = IMG_Load(filename.c_str());
  if (!raw) {
    __fatal
      << "Tileset not found: " << filename;
    exit(1);
  }
  SDL_Surface *src = canonicalize_format(raw, convert_magenta);
  SDL_SetAlpha(src, 0, 255);

  __info
    << "width: " << src->w << " dimx: " << dimx;

  *disp_x = src->w / dimx;
  *disp_y = src->h / dimy;
  long idx = 0;
  for (int y = 0; y < dimy; y++)
    for (int x = 0; x < dimx; x++) {
      SDL_Surface *tile = SDL_CreateRGBSurface(SDL_SWSURFACE, *disp_x, *disp_y, 32, src->format->Rmask,
          src->format->Gmask, src->format->Bmask, src->format->Amask);
      SDL_SetAlpha(tile, 0, 255);
      SDL_Rect pos_src;
      pos_src.x = *disp_x * x;
      pos_src.y = *disp_y * y;
      pos_src.w = *disp_x;
      pos_src.h = *disp_y;
      SDL_BlitSurface(src, &pos_src, tile, NULL);
      tex_pos[idx] = add_texture(tile);
      idx++;
    }
  // Re-upload textures if necessary
  enabler.reset_textures();
}

long textures::load(const ::std::string &filename, bool convert_magenta) {
  SDL_Surface *raw = IMG_Load(filename.c_str());
  if (!raw) {
    __error
      << "Image not found: " << filename;
    exit(1);
  }
  SDL_Surface *tex = canonicalize_format(raw, convert_magenta);
  long pos = add_texture(tex);
  // Re-upload if necessary
  enabler.reset_textures();
  return pos;
}

void textures::delete_texture(long pos) {
  // We can't actually resize the array, as
  // (a) it'd be slow, and
  // (b) it'd change all the positions. Bad stuff would happen.
  if (raws[pos]) {
    SDL_FreeSurface(raws[pos]);
    raws[pos] = NULL;
  }
}

void textures::grayscale_texture(long pos) {
  SDL_Surface *s = get_texture_data(pos);
  if (!s)
    return;
  SDL_LockSurface(s);
  SDL_PixelFormat *f = s->format;
  uint32_t *pixels = (uint32_t*) (s->pixels);
  if (f->BytesPerPixel != 4) {
    __error
      << "grayscale_texture ran into mysteriously uncanocalized texture";
    goto cleanup;
  }
  for (int i = 0; i < s->w * s->h; i++) { // For each pixel
    int r = (pixels[i] & f->Rmask) >> f->Rshift;
    int g = (pixels[i] & f->Gmask) >> f->Gshift;
    int b = (pixels[i] & f->Bmask) >> f->Bshift;
    int alpha = (pixels[i] & f->Amask) >> f->Ashift;
    int luminosity = (int) ((float) r * 0.30f + (float) g * 0.59f + (float) b * 0.11f);
    if (luminosity < 0)
      luminosity = 0;
    if (luminosity > 255)
      luminosity = 255;
    pixels[i] = (luminosity << f->Rshift) | (luminosity << f->Gshift) | (luminosity << f->Bshift)
        | (alpha << f->Ashift);
  }

  cleanup: SDL_UnlockSurface(s);

  enabler.reset_textures();
}
