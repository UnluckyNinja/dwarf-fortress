/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_RENDERER_2D_HPP_
#define DWARF_FORTRESS_RENDERER_2D_HPP_

#include "g_src/enabler.h"

#include <map>
#include <list>

class renderer_2d_base: public renderer {
  protected:
    SDL_Surface *screen;
    std::map< texture_fullid, SDL_Surface* > tile_cache;

    int dispx, dispy, dimx, dimy;

    // We may shrink or enlarge dispx/dispy in response to zoom requests. dispx/y_z are the
    // size we actually display tiles at.
    int dispx_z, dispy_z;

    // Viewport origin
    int origin_x, origin_y;

    SDL_Surface *tile_cache_lookup(texture_fullid &id);

    virtual bool init_video(int w, int h);

  public:
    std::list< std::pair< SDL_Surface*, SDL_Rect > > ttfs_to_render;

    void update_tile(int x, int y);
    void update_all();
    virtual void render();
    virtual ~renderer_2d_base();
    void grid_resize(int w, int h);
    renderer_2d_base();

    int zoom_steps, forced_steps;
    int natural_w, natural_h;

    void compute_forced_zoom();
    pair< int, int > compute_zoom(bool clamp = false);
    void resize(int w, int h);
    void reshape(pair< int, int > max_grid);

  private:
    void set_fullscreen();
    bool get_mouse_coords(int &x, int &y);
    void zoom(zoom_commands cmd);
};

class renderer_2d: public renderer_2d_base {
  public:
    renderer_2d();
};

#endif /* DWARF_FORTRESS_RENDERER_2D_HPP_ */
