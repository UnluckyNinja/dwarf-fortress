/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_OPENGL_HPP_
#define INTERN_RENDERER_OPENGL_HPP_

#include <map>

#include "extern/renderer.hpp"

class renderer_opengl: public renderer {
  public:
    virtual bool uses_opengl();

  protected:
    SDL_Surface *screen;

    int dispx, dispy; // Cache of the current font size

    bool init_video(int w, int h);

    // Vertexes, foreground color, background color, texture coordinates
    GLfloat *vertexes, *fg, *bg, *tex;

    void write_tile_vertexes(GLfloat x, GLfloat y, GLfloat *vertex);

    virtual void allocate(int tiles);
    virtual void init_opengl();
    virtual void uninit_opengl();
    virtual void draw(int vertex_count);

    void write_tile_arrays(int x, int y, GLfloat *fg, GLfloat *bg, GLfloat *tex);

  public:
    void update_tile(int x, int y);
    void update_all();
    void render();

    renderer_opengl();

    int zoom_steps, forced_steps;
    int natural_w, natural_h; // How large our view would be if it wasn't zoomed

    void zoom(zoom_commands cmd);

    void compute_forced_zoom();

    ::std::pair< int, int > compute_zoom(bool clamp = false);

    // Parameters: grid units
    void reshape(::std::pair< int, int > size);
    int off_x, off_y, size_x, size_y;
    bool get_mouse_coords(int &x, int &y);
    virtual void reshape_gl();

    // Parameters: window size
    void resize(int w, int h);
    // Parameters: grid size
    void grid_resize(int w, int h);

  public:
    void set_fullscreen();
};

#endif /* INTERN_RENDERER_OPENGL_HPP_ */
