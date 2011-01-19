/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/renderer/accum_buffer.hpp"

void renderer_accum_buffer::draw(int vertex_count) {
  // Copy the previous frame's buffer back in
glAccum(GL_RETURN, 1);
  renderer_once::draw(vertex_count);
  // Store the screen contents back to the buffer
glAccum(GL_LOAD, 1);
}
