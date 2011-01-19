/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/renderer/partial.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"
#include "extern/graphics.hpp"

#include <logging/logging.hpp>

#include <cassert>

void renderer_partial::update_tile(int x, int y) {
  write_tile_vertexes(x, y, vertexes + head * 6 * 2);
  write_tile_arrays(x, y, fg + head * 6 * 4, bg + head * 6 * 4, tex + head * 6 * 2);
  head = (head + 1) % buffersz;
  current_erasz++;
  sum_erasz++;
  if (head == tail) {
    //gamelog << "Expanding partial-printing buffer" << endl;
    // Buffer is full, expand it.
    renderer_opengl::allocate(buffersz * 2);
    // Move the tail to the end of the newly allocated space
    tail += buffersz;
    memmove(vertexes + tail * 6 * 2, vertexes + head * 6 * 2, sizeof(GLfloat) * 6 * 2 * (buffersz - head));
    memmove(fg + tail * 6 * 4, fg + head * 6 * 4, sizeof(GLfloat) * 6 * 4 * (buffersz - head));
    memmove(bg + tail * 6 * 4, fg + head * 6 * 4, sizeof(GLfloat) * 6 * 4 * (buffersz - head));
    memmove(tex + tail * 6 * 2, fg + head * 6 * 2, sizeof(GLfloat) * 6 * 2 * (buffersz - head));
    // And finish.
    buffersz *= 2;
  }
}

void renderer_partial::allocate(int tile_count) {
  assert(false);
}

void renderer_partial::reshape_gl() {
  // TODO: This function is duplicate code w/base class reshape_gl
  // Setup invariant state
  glEnableClientState(GL_COLOR_ARRAY);
  /// Set up our coordinate system
  if (forced_steps + zoom_steps == 0 && init.display.flag.has_flag(INIT_DISPLAY_FLAG_BLACK_SPACE)) {
    size_x = gps.dimx * dispx;
    size_y = gps.dimy * dispy;
    off_x = (screen->w - size_x) / 2;
    off_y = (screen->h - size_y) / 2;
  } else {
    // If we're zooming (or just not using black space), we use the
    // entire window.
    size_x = screen->w;
    size_y = screen->h;
    off_x = off_y = 0;
  }
  glViewport(off_x, off_y, size_x, size_y);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, gps.dimx, gps.dimy, 0);
}

void renderer_partial::draw_arrays(GLfloat *vertexes, GLfloat *fg, GLfloat *bg, GLfloat *tex, int tile_count) {
  // Set vertex pointer
  glVertexPointer(2, GL_FLOAT, 0, vertexes);
  // Render the background colors
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glColorPointer(4, GL_FLOAT, 0, bg);
  glDrawArrays(GL_TRIANGLES, 0, tile_count * 6);
  // Render the foreground, colors and textures both
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColorPointer(4, GL_FLOAT, 0, fg);
  glTexCoordPointer(2, GL_FLOAT, 0, tex);
  glDrawArrays(GL_TRIANGLES, 0, tile_count * 6);
}

void renderer_partial::draw(int dummy) {
  if (tail > head) {
    // We're straddling the end of the array, so have to do this in two steps
    draw_arrays(vertexes + tail * 6 * 2, fg + tail * 6 * 4, bg + tail * 6 * 4, tex + tail * 6 * 2, buffersz - tail);
    draw_arrays(vertexes, fg, bg, tex, head - 1);
  } else {
    draw_arrays(vertexes + tail * 6 * 2, fg + tail * 6 * 4, bg + tail * 6 * 4, tex + tail * 6 * 2, sum_erasz);
  }

  __gl_check_errors;
  erasz.push_back(current_erasz);
  current_erasz = 0;
  if (erasz.size() == (unsigned int) redraw_count) {
    // Right, time to retire the oldest era.
    tail = (tail + erasz.front()) % buffersz;
    sum_erasz -= erasz.front();
    erasz.pop_front();
  }
}

renderer_partial::renderer_partial() {
  redraw_count = init.display.partial_print_count;
  buffersz = 2048;
  renderer_opengl::allocate(buffersz);
  current_erasz = head = tail = sum_erasz = 0;
}
