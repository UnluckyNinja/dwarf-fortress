/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_RENDERER_HPP_
#define EXTERN_RENDERER_HPP_

#include "extern/enabler.hpp"
#include "meta/either.hpp"

struct texture_fullid;

// TODO: comment
// TODO: categorize
// Just the texpos
typedef int texture_ttfid;

class renderer {
  protected:
    unsigned char *screen;
    long *screentexpos;
    char *screentexpos_addcolor;
    unsigned char *screentexpos_grayscale;
    unsigned char *screentexpos_cf;
    unsigned char *screentexpos_cbr;
    // For partial printing:
    unsigned char *screen_old;
    long *screentexpos_old;
    char *screentexpos_addcolor_old;
    unsigned char *screentexpos_grayscale_old;
    unsigned char *screentexpos_cf_old;
    unsigned char *screentexpos_cbr_old;

    void gps_allocate(int x, int y);
    Either< texture_fullid, texture_ttfid > screen_to_texid(int x, int y);
  public:
    void display();
    virtual void update_tile(int x, int y) = 0;
    virtual void update_all() = 0;
    virtual void render() = 0;
    virtual void set_fullscreen();
    virtual void zoom(zoom_commands cmd);
    virtual void resize(int w, int h) = 0;
    virtual void grid_resize(int w, int h) = 0;
    void swap_arrays();
    renderer();
    virtual bool get_mouse_coords(int &x, int &y) = 0;
    virtual bool uses_opengl();
};

#endif /* EXTERN_RENDERER_HPP_ */
