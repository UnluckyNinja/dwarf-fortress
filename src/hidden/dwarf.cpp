/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "hidden/dwarf.hpp"

#include "hidden/global.hpp"
#include "hidden/viewscreen.hpp"
#include "hidden/tetris.hpp"

#include "extern/utils/random.hpp"
#include "extern/screen/keybinding.hpp"
#include "extern/screen/movieplayer.hpp"

#include "extern/interface.hpp"

char beginroutine() {
  mt_init();

  gview.addscreen(new matrix_screen(), INTERFACE_PUSH_AT_BACK, 0);
  return 1;
}

void endroutine() {
}

char mainloop() {
  return gview.loop();
}

//void dwarf_help_routine() {
//}

void dwarf_end_announcements() {
}

void dwarf_option_screen() {
  new KeybindingScreen();
}

void dwarf_remove_screen() {
}

void drawborder(const char* str, char style, const char* color) {
  gps.erasescreen();
  if (style) {
    gps.changecolor(7, 7, 0);
    for (int x = 0; x < gps.dimx; x++) {
      gps.locate(0, x);
      gps.addchar(' ');
      gps.locate(gps.dimy - 1, x);
      gps.addchar(' ');
    }

    for (int y = 1; y < gps.dimy - 1; y++) {
      gps.locate(y, 0);
      gps.addchar(' ');
      gps.locate(y, gps.dimx - 1);
      gps.addchar(' ');
    }
  }

  if (!str) {
    return;
  }

  gps.locate(0, gps.dimx / 2);
  if (color) {
    gps.addcoloredst(str, color);
  } else {
    gps.changecolor(7, 0, 1);
    // Uint32 now = SDL_GetTicks() % 3000;
    // gps.addst(str, now < 1000 ? justify_left : now < 2000 ? justify_center : justify_right);
    gps.addst(str, justify_center);
  }
}

void process_object_lines(textlinesst& lines, ::std::string& chktype, ::std::string& graphics_dir) {
}

char standardscrolling(::std::set< interface_key_t >& events, int32_t& selection, int32_t min, int32_t max, int32_t jump, uint32_t flag) {
  return 0;
}
