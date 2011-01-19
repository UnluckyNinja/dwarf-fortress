#include "hidden/tetris.hpp"

#include "hidden/dwarf.hpp"
#include "hidden/global.hpp"
#include "extern/texture_handler.hpp"
#include "extern/init.hpp"
#include "extern/interface.hpp"
#include "extern/graphics.hpp"
#include "extern/enabler.hpp"

#include <logging/logging.hpp>

#include "extern/utils/random.hpp"

bool tetriminoes[7][2][4] = { { { 0, 0, 0, 0 }, { 1, 1, 1, 1 } }, { { 1, 0, 0, 0 }, { 1, 1, 1, 0 } }, { { 0, 0, 0, 1 }, { 0, 1, 1, 1 } }, { { 0, 1, 1, 0 }, { 0, 1, 1, 0 } }, { { 0, 0, 1, 1 },
                                                                                                                                                                                { 0, 1, 1, 0 } },
                              { { 0, 0, 1, 0 }, { 0, 1, 1, 1 } }, { { 1, 1, 0, 0 }, { 0, 1, 1, 0 } } };

struct line {
    int x, y; // Location of the topmost character. y may be negative.
    ::std::string s;
    int color;
};

::std::string matrix_screen::random_string(int len) {
  ::std::string s(len, 0);
  for (int i = 0; i < len; i++) {
    s[i] = basic_random(256);
  }
  return s;
}

matrix_screen::matrix_screen() {
  frame = 0;
}

void matrix_screen::feed(::std::set< interface_key_t > &events) {
  breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
}

void matrix_screen::render() {
  gps.erasescreen();
  drawborder("Matrix", 1, 0);
  for (iterator_t i = lines.begin(); i != lines.end(); ++i) {
    for (::std::size_t j = 0; j < i->s.size(); ++j) {
      int x = i->x;
      int y = i->y + j;
      if (x < gps.dimx && y > 0 && y < gps.dimy)
        gps.addchar(i->x, i->y + j, i->s[j], i->color, 0, 0);
    }
  }
}

void matrix_screen::logic() {
  if ((frame++) % 2) {
    return;
  }

  enabler.flag |= ENABLERFLAG_RENDER;
  // Advance each line one step
  for (iterator_t i = lines.begin(); i != lines.end();) {
    if ((++(i->y)) >= gps.dimy) {
      lines.erase(i++);
    } else {
      ++i;
    }
  }
  // Maybe add a new line
  if (basic_random(100) < 30) {
    line l;
    l.x = basic_random(gps.dimx);
    int len = basic_random(gps.dimy);
    l.y = -len;
    l.s = random_string(len);
    l.color = basic_random(7) + 1;
    lines.push_back(l);
  }
}

void matrix_screen::resize(int w, int h) {
  // Clear out lines that are now off-screen
  for (iterator_t i = lines.begin(); i != lines.end();) {
    if (i->x >= w || i->y >= h) {
      lines.erase(i++);
    } else {
      ++i;
    }
  }
}
