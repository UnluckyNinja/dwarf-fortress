/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#define BOOST_PARAMETER_MAX_ARITY 7

#include "intern/screen/macro_save.hpp"

#include "hidden/global.hpp"
#include "extern/graphics.hpp"
#include "extern/init.hpp"
#include "extern/enabler.hpp"
#include "extern/interface.hpp"

#include "intern/enabler/input.hpp"

MacroScreenSave::MacroScreenSave() {
  enabler.flag |= ENABLERFLAG_RENDER;
}

void MacroScreenSave::logic() {
}

void MacroScreenSave::render() {
  if (parent)
    parent->render();
  const int x1 = 3, x2 = init.display.grid_x - 4, y1 = init.display.grid_y / 2 - 1, y2 = init.display.grid_y / 2 + 1;
  gps.changecolor(0, 3, 1);
  gps.draw_border(x1, x2, y1, y2);
  id.render(x1 + 1, x2 - 1, y1 + 1, y2 - 1);
  // gps.renewscreen();
}

void MacroScreenSave::feed(::std::set< interface_key_t > &input) {
  enabler.flag |= ENABLERFLAG_RENDER;
  id.feed(input);
  if (input.count(INTERFACEKEY_SELECT)) {
    ::std::string n = id.get_text();
    if (n.length())
      enabler_input::instance().save_macro(n);
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  }
  if (input.count(INTERFACEKEY_OPTIONS)) {
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
  }
}

char MacroScreenSave::is_option_screen() {
  return 1;
}
