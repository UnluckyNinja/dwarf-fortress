/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_RENDERER_BASE_HPP_
#define EXTERN_RENDERER_BASE_HPP_

#include <map>

#include "extern/renderer.hpp"

class renderer_2d_base: public renderer {
  protected:
    SDL_Surface *screen;
    ::std::map< texture_fullid, SDL_Surface* > tile_cache;
    int dispx, dispy, dimx, dimy;
    // We may shrink or enlarge dispx/dispy in response to zoom requests. dispx/y_z are the
    // size we actually display tiles at.
    int dispx_z, dispy_z;
    // Viewport origin
    int origin_x, origin_y;

    SDL_Surface *tile_cache_lookup(texture_fullid &id);
    virtual bool init_video(int w, int h);

  public:
    ::std::list< ::std::pair< SDL_Surface*, SDL_Rect > > ttfs_to_render;

    void update_tile(int x, int y);
    void update_all();

    virtual void render();

    ~renderer_2d_base();

    void grid_resize(int w, int h);

    renderer_2d_base();
    int zoom_steps, forced_steps;
    int natural_w, natural_h;

    void compute_forced_zoom();

    ::std::pair< int, int > compute_zoom(bool clamp = false);

    void resize(int w, int h);

    void reshape(::std::pair< int, int > max_grid);

  private:

    void set_fullscreen();

    bool get_mouse_coords(int &x, int &y);

    void zoom(zoom_commands cmd);
};

#endif /* EXTERN_RENDERER_BASE_HPP_ */
