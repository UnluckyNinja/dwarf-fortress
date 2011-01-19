/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_RENDERER_FRAMEBUFFER_HPP_
#define INTERN_RENDERER_FRAMEBUFFER_HPP_

#include "intern/renderer/once.hpp"

class renderer_framebuffer: public renderer_once {
    GLuint framebuffer, fb_texture;

    void init_opengl();
    void uninit_opengl();
    void draw(int vertex_count);
};

#endif /* INTERN_RENDERER_FRAMEBUFFER_HPP_ */
