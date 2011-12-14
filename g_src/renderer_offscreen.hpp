/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_RENDERER_OFFSCREEN_HPP_
#define DWARF_FORTRESS_RENDERER_OFFSCREEN_HPP_

#include "renderer_2d.hpp"

#include <string>

class renderer_offscreen: public renderer_2d_base {
    virtual bool init_video(int, int);
  public:
    ~renderer_offscreen();
    renderer_offscreen(int, int);
    void update_all(int, int);
    void save_to_file(const std::string &file);
};

#endif /* DWARF_FORTRESS_RENDERER_OFFSCREEN_HPP_ */
