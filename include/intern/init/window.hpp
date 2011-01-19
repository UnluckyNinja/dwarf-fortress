/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_INIT_WINDOW_HPP_
#define INTERN_INIT_WINDOW_HPP_

#include "extern/flag_array.hpp"

enum InitWindowFlag {
  INIT_WINDOW_FLAG_TOPMOST,
  INIT_WINDOW_FLAG_VSYNC_ON,
  INIT_WINDOW_FLAG_VSYNC_OFF,
  INIT_WINDOW_FLAG_TEXTURE_LINEAR,
  INIT_WINDOW_FLAGNUM
};

class init_windowst {
  public:
    flagarrayst flag;

    init_windowst();
};

#endif /* INTERN_INIT_WINDOW_HPP_ */
