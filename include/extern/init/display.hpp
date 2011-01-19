/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_INIT_DISPLAY_HPP_
#define EXTERN_INIT_DISPLAY_HPP_

enum InitDisplayFlag {
  INIT_DISPLAY_FLAG_USE_GRAPHICS,
  INIT_DISPLAY_FLAG_BLACK_SPACE,
  INIT_DISPLAY_FLAG_PARTIAL_PRINT,
  INIT_DISPLAY_FLAG_FRAME_BUFFER,
  INIT_DISPLAY_FLAG_SINGLE_BUFFER,
  INIT_DISPLAY_FLAG_ACCUM_BUFFER,
  INIT_DISPLAY_FLAG_VBO,
  INIT_DISPLAY_FLAG_2D,
  INIT_DISPLAY_FLAG_2DHW,
  INIT_DISPLAY_FLAG_2DASYNC,
  INIT_DISPLAY_FLAG_UNUSED_01_08,//
  INIT_DISPLAY_FLAG_TEXT,
  INIT_DISPLAY_FLAG_SHADER,
  INIT_DISPLAY_FLAG_NOT_RESIZABLE,
  INIT_DISPLAY_FLAG_ARB_SYNC,
  INIT_DISPLAY_FLAGNUM
};

enum InitDisplayWindow {
  INIT_DISPLAY_WINDOW_TRUE, INIT_DISPLAY_WINDOW_FALSE, INIT_DISPLAY_WINDOW_PROMPT, INIT_DISPLAY_WINDOWNUM
};

#include "extern/flag_array.hpp"

class init_displayst {
  public:
    flagarrayst flag;
    InitDisplayWindow windowed;

    int grid_x, grid_y; // The *current* display grid size, kept up to date

    int desired_fullscreen_width, desired_fullscreen_height;
    int desired_windowed_width, desired_windowed_height;

    char partial_print_count;

    /**
     * @ingroup extern
     */
    init_displayst();
};

#endif /* EXTERN_INIT_DISPLAY_HPP_ */
