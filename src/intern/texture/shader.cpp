/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/texture/shader.hpp"

#include <logging/logging.hpp>

#include <fstream>

void shader::load(const ::std::string &filename) {
  this->filename = filename;
  ::std::ifstream file(filename.c_str());
  ::std::string version;
  getline(file, version);
  header << version << ::std::endl;
  while (file.good()) {
    ::std::string line;
    getline(file, line);
    lines << line << ::std::endl;
  }
  file.close();
}

GLuint shader::upload(GLenum type) {
  GLuint shader = glCreateShader(type);
  ::std::string lines_done = lines.str(), header_done = header.str();
  const char *ptrs[3];
  ptrs[0] = header_done.c_str();
  ptrs[1] = "#line 1 0\n";
  ptrs[2] = lines_done.c_str();
  glShaderSource(shader, 3, ptrs, NULL);
  glCompileShader(shader);
  // Let's see if this compiled correctly..
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) { // ..no. Check the compilation log.
    GLint log_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    __error << filename << " preprocessed source:";
    __error << header_done << "#line 1 0\n" << lines_done;
    __error << filename << " shader compilation log (" << log_size << "):";
    char *buf = new char[log_size];
    glGetShaderInfoLog(shader, log_size, NULL, buf);
    __error << buf;
    delete[] buf;
    __fatal << "Shader compilation failed; details in errorlog.txt";
    abort();
  }
  __gl_check_errors;
  return shader;
}
