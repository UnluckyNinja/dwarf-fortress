/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_INITST_HPP_
#define EXTERN_INITST_HPP_

#include "extern/init/display.hpp"
#include "intern/init/media.hpp"
#include "intern/init/input.hpp"
#include "intern/init/font.hpp"
#include "intern/init/window.hpp"

class initst {
  public:
    init_displayst display;
    init_mediast media;
    init_inputst input;
    init_fontst font;
    init_windowst window;

    /**
     * @ingroup extern
     */
    void begin();
};

#endif /* EXTERN_INITST_HPP_ */
