/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/init/input.hpp"

init_inputst::init_inputst() {
  hold_time = 150;
  repeat_time = 150;
  macro_time = 75;
  pause_zoom_no_interface_ms = 0;
  flag.set_size_on_flag_num(INIT_INPUT_FLAGNUM);
  zoom_speed = 10;
  repeat_accel_start = 10;
  repeat_accel_limit = 1;
}
