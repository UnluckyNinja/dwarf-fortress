/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#define BOOST_PARAMETER_MAX_ARITY 7

#include "intern/screen/macro_load.hpp"

#include "hidden/global.hpp"
#include "extern/enabler.hpp"
#include "extern/init.hpp"

#include "extern/interface.hpp"

#include "intern/enabler/input.hpp"

#include <list>

MacroScreenLoad::MacroScreenLoad() {
  ::std::list< ::std::string > macros = enabler_input::instance().list_macros();
  width = 10;
  if (!macros.size()) {
    menu.add("No macros!", "");
    height = 1;
  } else
    height = macros.size();

  for (::std::list< ::std::string >::iterator it = macros.begin(); it != macros.end(); ++it) {
    if (it->length() > (unsigned int) width)
      width = it->length();
    menu.add(*it, *it);
  }
  enabler.flag |= ENABLERFLAG_RENDER;
  // render();
  // gps.renewscreen();
}

void MacroScreenLoad::logic() {
}

void MacroScreenLoad::render() {
  if (parent)
    parent->render();
  const int x1 = ::std::max(init.display.grid_x / 2 - ((width + 2) / 2), 0);
  const int x2 = ::std::min(x1 + width + 1, init.display.grid_x - 1);
  const int y1 = ::std::max(init.display.grid_y / 2 - ((height + 2) / 2), 0);
  const int y2 = ::std::min(y1 + height + 1, init.display.grid_y - 1);
  gps.changecolor(0, 3, 1);
  gps.draw_border(x1, x2, y1, y2);
  menu.render(x1 + 1, x2 - 1, y1 + 1, y2 - 1);
  // gps.renewscreen();
}

void MacroScreenLoad::feed(::std::set< interface_key_t > &input) {
  enabler.flag |= ENABLERFLAG_RENDER;
  if (input.count(INTERFACEKEY_SELECT)) {
    ::std::string id = menu.get_selection();
    if (id != "")
      enabler_input::instance().load_macro(id);
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  } else if (input.count(INTERFACEKEY_LEAVESCREEN)) {
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  } else {
    menu.feed(input);
  }
  if (input.count(INTERFACEKEY_OPTIONS)) {
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
  }
}

char MacroScreenLoad::is_option_screen() {
  return 1;
}
