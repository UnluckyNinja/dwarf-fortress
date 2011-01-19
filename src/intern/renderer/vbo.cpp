/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/renderer/vbo.hpp"
#include "hidden/global.hpp"
#include "extern/graphics.hpp"

void renderer_vbo::init_opengl() {
  renderer_opengl::init_opengl();
  glGenBuffersARB(1, &vbo);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, gps.dimx * gps.dimy * 6 * 2 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW_ARB);
  glVertexPointer(2, GL_FLOAT, 0, 0);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void renderer_vbo::uninit_opengl() {
  glDeleteBuffersARB(1, &vbo);
  renderer_opengl::uninit_opengl();
}
