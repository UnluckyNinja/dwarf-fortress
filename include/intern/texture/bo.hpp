/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXTURE_BO_HPP_
#define INTERN_TEXTURE_BO_HPP_

#include <GL/glew.h>

class texture_bo {
    GLuint bo, tbo;
  public:
    texture_bo();
    void reset();
    void buffer(GLvoid *ptr, GLsizeiptr sz);
    void bind(GLenum texture_unit, GLenum type);
    GLuint texnum();
};

#endif /* INTERN_TEXTURE_BO_HPP_ */
