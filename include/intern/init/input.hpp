/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_INIT_INPUT_HPP_
#define INTERN_INIT_INPUT_HPP_

#include "extern/flag_array.hpp"

enum InitInputFlag {
  INIT_INPUT_FLAG_MOUSE_OFF, INIT_INPUT_FLAG_MOUSE_PICTURE, INIT_INPUT_FLAGNUM
};

class init_inputst {
  public:
    long hold_time;
    long repeat_time;
    long macro_time;
    long pause_zoom_no_interface_ms;
    flagarrayst flag;
    long zoom_speed;
    int repeat_accel_start;
    int repeat_accel_limit;

    init_inputst();
};

#endif /* INTERN_INIT_INPUT_HPP_ */
