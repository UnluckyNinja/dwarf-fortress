/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/init/display.hpp"

init_displayst::init_displayst() {
  flag.set_size_on_flag_num(INIT_DISPLAY_FLAGNUM);
  windowed = INIT_DISPLAY_WINDOW_PROMPT;

  partial_print_count = 0;
}
