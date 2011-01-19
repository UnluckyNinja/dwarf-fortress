/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_ACCUM_BUFFER_HPP_
#define INTERN_RENDERER_ACCUM_BUFFER_HPP_

#include "intern/renderer/once.hpp"

class renderer_accum_buffer: public renderer_once {
    void draw(int vertex_count);
};

#endif /* INTERN_RENDERER_ACCUM_BUFFER_HPP_ */
