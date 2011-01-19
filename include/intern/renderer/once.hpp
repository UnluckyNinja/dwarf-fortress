/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_ONCE_HPP_
#define INTERN_RENDERER_ONCE_HPP_

#include "intern/renderer/opengl.hpp"

// Specialization for PARTIAL:0
class renderer_once: public renderer_opengl {
    int tile_count;

  protected:
    void update_tile(int x, int y);
    void draw(int dummy);

  public:
    renderer_once();
};

#endif /* INTERN_RENDERER_ONCE_HPP_ */
