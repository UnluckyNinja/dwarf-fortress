/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_SCREEN_MACRO_LOAD_HPP_
#define INTERN_SCREEN_MACRO_LOAD_HPP_

#include <set>

#include <string>
#include "event_types.hpp"

#include "hidden/viewscreen.hpp"

#include "intern/screen/widget/menu.hpp"

class MacroScreenLoad: public viewscreenst {
    ::widgets::menu< ::std::string > menu;
    int width, height;

  public:
    MacroScreenLoad();
    virtual void logic();
    virtual void render();
    virtual void feed(::std::set< interface_key_t > &events);
    virtual char is_option_screen();
};

#endif /* INTERN_SCREEN_MACRO_LOAD_HPP_ */
