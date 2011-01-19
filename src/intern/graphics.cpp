/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/graphics.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"

graphicst gps;

void graphicst::resize(int x, int y) {
  dimx = x;
  dimy = y;
  init.display.grid_x = x;
  init.display.grid_y = y;
  setclipping(0, x - 1, 0, y - 1);
  force_full_display_count++;
  screen_limit = screen + dimx * dimy * 4;
}

graphicst::graphicst() {
  print_index = 0;
  display_frames = 0;
  rect_id = -1;
  force_full_display_count = 4;
  original_rect = 1;

  screentexpos = NULL;
  screentexpos_addcolor = NULL;
  screentexpos_grayscale = NULL;
  screentexpos_cf = NULL;
  screentexpos_cbr = NULL;
  screen = NULL;
}

void graphicst::locate(long y, long x) {
  // No point in clamping here, addchar clamps too.
  screenx = x;
  screeny = y;
}

void graphicst::changecolor(short f, short b, char bright) {
  screenf = f;
  screenb = b;
  screenbright = bright;
}

void graphicst::addchar(unsigned char c, char advance) {
  /* assert (screen_limit == screen + dimy * dimx * 4); */
  unsigned char *s = screen + screenx * dimy * 4 + screeny * 4;
  if (s < screen_limit) {
    *s++ = c;
    *s++ = screenf;
    *s++ = screenb;
    *s++ = screenbright;
    screentexpos[screenx * dimy + screeny] = 0;
  }
  if (advance)
    screenx++;
}

void graphicst::addchar(unsigned int x, unsigned int y, unsigned char c, unsigned char f, unsigned char b, unsigned char bright) {
  /* assert (screen_limit == screen + dimy * dimx * 4); */
  unsigned char *s = screen + x * dimy * 4 + y * 4;
  if (s >= screen && s < screen_limit) {
    *s++ = c;
    *s++ = f;
    *s++ = b;
    *s++ = bright;
  }
}

void graphicst::erasescreen_clip() {
  changecolor(0, 0, 0);
  short x2, y2;
  for (x2 = clipx[0]; x2 <= clipx[1]; x2++) {
    for (y2 = clipy[0]; y2 <= clipy[1]; y2++) {
      locate(y2, x2);
      addchar(' ');
    }
  }
}

void graphicst::erasescreen_rect(int x1, int x2, int y1, int y2) {
  changecolor(0, 0, 0);
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      locate(y, x);
      addchar(' ');
    }
  }
}

void graphicst::gray_out_rect(long sx, long ex, long sy, long ey) {
  long x, y;
  for (x = sx; x <= ex; x++) {
    for (y = sy; y <= ey; y++) {
      screen[x * dimy * 4 + y * 4 + 1] = 0;
      screen[x * dimy * 4 + y * 4 + 2] = 7;
      screen[x * dimy * 4 + y * 4 + 3] = 0;
    }
  }
}

void graphicst::rain_color_square(long x, long y) {
  if (x >= clipx[0] && x <= clipx[1] && y >= clipy[0] && y <= clipy[1]) {
    screen[x * dimy * 4 + y * 4 + 1] = 1;
    screen[x * dimy * 4 + y * 4 + 2] = 0;
    screen[x * dimy * 4 + y * 4 + 3] = 1;
  }
}

void graphicst::snow_color_square(long x, long y) {
  if (x >= clipx[0] && x <= clipx[1] && y >= clipy[0] && y <= clipy[1]) {
    screen[x * dimy * 4 + y * 4 + 1] = 7;
    screen[x * dimy * 4 + y * 4 + 2] = 0;
    screen[x * dimy * 4 + y * 4 + 3] = 1;
  }
}

long graphicst::border_start_x() {
  return 1;
}

long graphicst::border_start_y() {
  return 1;
}

long graphicst::border_end_x() {
  return 78;
}

long graphicst::border_end_y() {
  return 23;
}

long graphicst::text_width() {
  return 1;
}

long graphicst::text_height() {
  return 1;
}

long graphicst::window_element_height(long minus, char border) {
  long height = 25;
  if (border)
    height -= 2;
  height -= text_height() * minus;
  return height;
}

void graphicst::draw_border(int x1, int x2, int y1, int y2) {
  // Upper and lower
  for (int x = x1; x <= x2; x++) {
    locate(y1, x);
    addchar(' ');
    locate(y2, x);
    addchar(' ');
  }
  // Left and right
  for (int y = y1 + 1; y < y2; y++) {
    locate(y, x1);
    addchar(' ');
    locate(y, x2);
    addchar(' ');
  }
}
