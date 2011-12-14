/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_RENDERER_OPENGL_HPP_
#define DWARF_FORTRESS_RENDERER_OPENGL_HPP_

#include "g_src/enabler.h"

#include <map>
#include <list>

// STANDARD
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
    virtual ~renderer_opengl() {
    }

    int zoom_steps, forced_steps;
    int natural_w, natural_h; // How large our view would be if it wasn't zoomed

    void zoom(zoom_commands cmd);
    void compute_forced_zoom();
    std::pair< int, int > compute_zoom(bool clamp = false);

    // Parameters: grid units
    void reshape(std::pair< int, int > size);

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

// Specialization for PARTIAL:0
class renderer_once: public renderer_opengl {
    int tile_count;

  protected:
    void update_tile(int x, int y);
    void draw(int dummy);

  public:
    renderer_once();
    virtual ~renderer_once() {
    }
};

// PARTIAL:N
class renderer_partial: public renderer_opengl {
    int buffersz;
    std::list< int > erasz; // Previous eras
    int current_erasz; // And the current one
    int sum_erasz;
    int head, tail; // First unused tile, first used tile respectively
    int redraw_count; // Number of eras to max out at

    void update_tile(int x, int y);
    void allocate(int tile_count);
    virtual void reshape_gl();
    void draw_arrays(GLfloat *vertexes, GLfloat *fg, GLfloat *bg, GLfloat *tex, int tile_count);
    void draw(int dummy);

  public:
    renderer_partial();
    virtual ~renderer_partial() {
    }
};

class renderer_accum_buffer: public renderer_once {
    void draw(int vertex_count);
    virtual ~renderer_accum_buffer() {
    }
};

class renderer_framebuffer: public renderer_once {
    GLuint framebuffer, fb_texture;

    void init_opengl();
    void uninit_opengl();
    void draw(int vertex_count);
    virtual ~renderer_framebuffer() {
    }
};

class renderer_vbo: public renderer_opengl {
    GLuint vbo; // Vertexes only

    void init_opengl();
    void uninit_opengl();
    virtual ~renderer_vbo() {
    }
};

#endif /* DWARF_FORTRESS_RENDERER_OPENGL_HPP_ */
