/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/texture/bo.hpp"

texture_bo::texture_bo() {
  bo = tbo = 0;
}

void texture_bo::reset() {
  if (bo) {
    glDeleteBuffers(1, &bo);
    glDeleteTextures(1, &tbo);
    bo = tbo = 0;
    __gl_check_errors;
  }
}

void texture_bo::buffer(GLvoid *ptr, GLsizeiptr sz) {
  if (bo)
    reset();
  glGenBuffersARB(1, &bo);
  glGenTextures(1, &tbo);
  glBindBufferARB(GL_TEXTURE_BUFFER_ARB, bo);
  glBufferDataARB(GL_TEXTURE_BUFFER_ARB, sz, ptr, GL_STATIC_DRAW_ARB);
  __gl_check_errors;
}

void texture_bo::bind(GLenum texture_unit, GLenum type) {
  glActiveTexture(texture_unit);
  glBindTexture(GL_TEXTURE_BUFFER_ARB, tbo);
  glTexBufferARB(GL_TEXTURE_BUFFER_ARB, type, bo);
  __gl_check_errors;
}

GLuint texture_bo::texnum() {
  return tbo;
}
