/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_UTILS_OPENGL_HPP_
#define INTERN_UTILS_OPENGL_HPP_

#include <GL/glew.h>

#include <logging/logging.hpp>

#define __gl_check_errors \
  while (GLenum __gl_error = glGetError()) { \
    __errorM(opengl) << "#0x" << ::std::hex << __gl_error << ::std::dec; \
  } \

#endif /* INTERN_UTILS_OPENGL_HPP_ */
