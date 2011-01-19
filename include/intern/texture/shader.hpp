/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_SHADER_HPP_
#define INTERN_SHADER_HPP_

#include <GL/glew.h>

#include <string>
#include <sstream>

class shader {
    ::std::string filename;
    ::std::ostringstream lines;
  public:
    ::std::ostringstream header;
    void load(const ::std::string &filename);
    GLuint upload(GLenum type);
};

#endif /* INTERN_SHADER_HPP_ */
