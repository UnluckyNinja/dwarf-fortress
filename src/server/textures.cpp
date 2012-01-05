#include <cassert>
#include <map>

#include "src/common/message/display.hpp"
#include "server.hpp"

#include "g_src/enabler.h"
#include "g_src/init.h"

namespace df {

  static SDL_Surface* canonicalize_format(SDL_Surface* source, bool convert_magenta) {
    SDL_PixelFormat format;
    format.palette = NULL;
    format.BitsPerPixel = 32;
    format.BytesPerPixel = 4;
    format.Rloss = 0;
    format.Gloss = 0;
    format.Bloss = 0;
    format.Aloss = 0;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    format.Rshift = 24;
    format.Gshift = 16;
    format.Bshift = 8;
    format.Ashift = 0;
#else
    format.Rshift = 0;
    format.Gshift = 8;
    format.Bshift = 16;
    format.Ashift = 24;
#endif

    format.Rmask = 0xFF << format.Rshift;
    format.Gmask = 0xFF << format.Gshift;
    format.Bmask = 0xFF << format.Bshift;
    format.Amask = 0xFF << format.Ashift;
    format.colorkey = 0;
    format.alpha = 0xFF;

    SDL_Surface* target = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                               source->w,
                                               source->h,
                                               32,
                                               format.Rmask,
                                               format.Gmask,
                                               format.Bmask,
                                               format.Amask);
    if (source->format->Amask == 0 && convert_magenta) {
      SDL_SetColorKey(source, SDL_SRCCOLORKEY, SDL_MapRGB(source->format, 255, 0, 255));
    }

    target = SDL_ConvertSurface(source, &format, SDL_SWSURFACE);
    SDL_FreeSurface(source);

    return target;
  }

  typedef df::message::tileset_t tileset_t;
  static std::vector< tileset_t > tilesets;
  static std::vector< tileset_t > gray_tilesets;

  tileset_t& load(std::string const& filename,
                  bool convert_magenta = false,
                  std::uint8_t const tile_count_x = 1,
                  std::uint8_t const tile_count_y = 1) {
    std::cerr << "df::load file: " << filename << " convert: " << convert_magenta << " " << (int) tile_count_x << "x"
        << (int) tile_count_y << "\n";

    SDL_Surface* surface = df::canonicalize_format(IMG_Load(filename.c_str()), convert_magenta);
    SDL_SetAlpha(surface, 0, 255);

    tilesets.push_back(tileset_t());
    tileset_t& tileset = tilesets.back();

    tileset.tileset_id_ = (tilesets.size() - 1) << 1;
    tileset.tile_count_x_ = tile_count_x;
    tileset.tile_count_y_ = tile_count_y;
    tileset.tile_size_x_ = surface->w / tileset.tile_count_x_;
    tileset.tile_size_y_ = surface->h / tileset.tile_count_y_;

    SDL_LockSurface(surface);
    SDL_PixelFormat* f = surface->format;
    Uint32* pixels = static_cast< Uint32* >(surface->pixels);
    for (std::size_t i = 0; i < surface->w * surface->h; i++) {
      std::uint32_t pixel = 0;
      pixel |= ((pixels[i] & f->Rmask) >> f->Rshift) << 24;
      pixel |= ((pixels[i] & f->Gmask) >> f->Gshift) << 16;
      pixel |= ((pixels[i] & f->Bmask) >> f->Bshift) << 8;
      pixel |= ((pixels[i] & f->Amask) >> f->Ashift) << 0;
      tileset.pixels.push_back(pixels[i]);
    }
    SDL_UnlockSurface(surface);

    SDL_FreeSurface(surface);

    return tileset;
  }

  tileset_t& grayscale(std::uint16_t const tileset_id) {
    std::uint16_t tileset_index = tileset_id >> 1;
    for (std::size_t index = gray_tilesets.size(); index < tileset_index + 1; ++index) {
      gray_tilesets.push_back(tilesets[index]);
    }

    df::tileset_t& tileset = gray_tilesets[tileset_index];
    if (tileset.tileset_id_ != (tileset_id | 1)) {
      df::tileset_t& other = df::tilesets[tileset_index];
      tileset.tileset_id_ = tileset_id | 1;
      tileset.tile_count_x_ = other.tile_count_x_;
      tileset.tile_count_y_ = other.tile_count_y_;
      tileset.tile_size_x_ = other.tile_size_x_;
      tileset.tile_size_y_ = other.tile_size_y_;
      tileset.pixels.assign(other.pixels.begin(), other.pixels.end());

      for (std::uint32_t& pixel : tileset.pixels) {
        std::uint8_t r = (pixel >> 24) & 0xFF;
        std::uint8_t g = (pixel >> 16) & 0xFF;
        std::uint8_t b = (pixel >> 8) & 0xFF;
        std::uint8_t a = (pixel >> 0) & 0xFF;

        std::uint8_t l = static_cast< std::uint8_t >(0.30 * r + 0.59 * g + 0.11 * b);
        pixel = (l << 24) | (l << 16) | (l << 8) | (a << 0);
      }
    }

    return tileset;
  }

} // namespace df

void textures::upload_textures() {
  df::message::display_t message;
  message.type = df::message::display::tileset_info;
  message.tileset_count = df::tilesets.size() + df::gray_tilesets.size();
  message.tilesets.insert(message.tilesets.end(), df::tilesets.begin(), df::tilesets.end());
  message.tilesets.insert(message.tilesets.end(), df::gray_tilesets.begin(), df::gray_tilesets.end());

  df::server::instance().send(message);
  uploaded = true;
}

void textures::remove_uploaded_textures() {
  std::cerr << "textures::remove_uploaded_textures\n";
  uploaded = false;
}

SDL_Surface* textures::get_texture_data(long pos) {
  std::uint16_t tileset_id = (pos >> 16) & 0xFFFF;
  std::cerr << "textures::get_texture_data tileset: " << tileset_id << "tile: " << (pos & 0xFFFF) << "\n";

  return 0;
}

long textures::clone_texture(long src) {
  std::uint16_t tileset_id = (src >> 16) & 0xFFFF;
//  std::cerr << "textures::clone_texture tileset: " << tileset_id << " tile: " << (src & 0xFFFF) << "\n";

  df::tileset_t& tileset = df::grayscale(tileset_id);
  return ((tileset.tileset_id_ & 0xFFFF) << 16) | (src & 0xFFFF);
}

void textures::grayscale_texture(long pos) {
  std::uint16_t tileset_id = (pos >> 16) & 0xFFFF;
//  std::cerr << "textures::grayscale_texture tileset: " << tileset_id << " tile: " << (pos & 0xFFFF) << "\n";
}

long textures::add_texture(SDL_Surface* surface) {
  std::cerr << "textures::add_texture\n";
  return 0;
}

void textures::load_multi_pdim(const string &filename,
                               long* tex_pos,
                               long dimx,
                               long dimy,
                               bool convert_magenta,
                               long* disp_x,
                               long* disp_y) {
  df::tileset_t& tileset = df::load(filename, convert_magenta, dimx, dimy);

  *disp_x = tileset.tile_size_x_;
  *disp_y = tileset.tile_size_y_;

  long idx = 0;
  for (std::uint8_t y = 0; y < tileset.tile_count_y_; y++) {
    for (std::uint8_t x = 0; x < tileset.tile_count_x_; x++) {
      tex_pos[idx] = (tileset.tileset_id_ & 0xFFFF) << 16;
      tex_pos[idx] |= (y & 0xFF) << 8;
      tex_pos[idx] |= (x & 0xFF);
      idx++;
    }
  }

  // Re-upload textures if necessary
  enabler.reset_textures();
}

long textures::load(const string &filename, bool convert_magenta) {
  df::tileset_t& tileset = df::load(filename, convert_magenta);

  long pos = (tileset.tileset_id_ & 0xFFFF) << 16;
  pos |= (1 & 0xFF) << 8;
  pos |= (1 & 0xFF);

  // Re-upload textures if necessary
  enabler.reset_textures();
  return pos;
}

void textures::delete_texture(long pos) {
}
