/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/renderer/framebuffer.hpp"

void renderer_framebuffer::init_opengl() {
  glGenFramebuffersEXT(1, &framebuffer);
  // Allocate FBO texture memory
  glGenTextures(1, &fb_texture);
  glBindTexture(GL_TEXTURE_2D, fb_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen->w, screen->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // Bind texture to FBO
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
  glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fb_texture, 0);
  renderer_once::init_opengl();
}

void renderer_framebuffer::uninit_opengl() {
  renderer_once::uninit_opengl();
  glDeleteTextures(1, &fb_texture);
  glDeleteFramebuffersEXT(1, &framebuffer);
}

void renderer_framebuffer::draw(int vertex_count) {
  // Bind the framebuffer
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, framebuffer);
  glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
  // Draw
  renderer_once::draw(vertex_count);
  // Draw the framebuffer to screen
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
  glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer);
  glBlitFramebufferEXT(0, 0, screen->w, screen->h, 0, 0, screen->w, screen->h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  __gl_check_errors;
}
