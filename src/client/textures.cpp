#ifndef __x86_64__

#include <string>

#include "g_src/enabler.h"

void textures::upload_textures() {
  uploaded = true;
}

void textures::remove_uploaded_textures() {
  uploaded = false;
}

SDL_Surface* textures::get_texture_data(long pos) {
  return 0;
}

long textures::clone_texture(long src) {
  return 0;
}

void textures::grayscale_texture(long pos) {
}

long textures::add_texture(SDL_Surface *surface) {
  return 0;
}

void textures::load_multi_pdim(const std::string &filename,
                               long *tex_pos,
                               long dimx,
                               long dimy,
                               bool convert_magenta,
                               long *disp_x,
                               long *disp_y) {
}

long textures::load(const std::string &filename, bool convert_magenta) {
  return 0;
}

void textures::delete_texture(long pos) {
}

#endif
