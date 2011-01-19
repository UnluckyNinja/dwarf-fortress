/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_VBO_HPP_
#define INTERN_RENDERER_VBO_HPP_

#include "intern/renderer/opengl.hpp"

class renderer_vbo: public renderer_opengl {
    GLuint vbo; // Vertexes only

    void init_opengl();
    void uninit_opengl();
};

#endif /* INTERN_RENDERER_VBO_HPP_ */
