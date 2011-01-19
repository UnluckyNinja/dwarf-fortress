/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_PARTIAL_HPP_
#define INTERN_RENDERER_PARTIAL_HPP_

#include <list>

#include "intern/renderer/opengl.hpp"

// PARTIAL:N
class renderer_partial: public renderer_opengl {
    int buffersz;
    ::std::list< int > erasz; // Previous eras
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
};

#endif /* INTERN_RENDERER_PARTIAL_HPP_ */
