/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_SCREEN_MACRO_SAVE_HPP_
#define INTERN_SCREEN_MACRO_SAVE_HPP_

#include <set>

#include <string>
#include "event_types.hpp"

#include "hidden/viewscreen.hpp"

#include "intern/screen/widget/textbox.hpp"

class MacroScreenSave: public viewscreenst {
    ::widgets::textbox id;
  public:
    MacroScreenSave();
    virtual void logic();
    virtual void render();
    virtual void feed(::std::set< interface_key_t > &events);
    virtual char is_option_screen();
};

#endif /* INTERN_SCREEN_MACRO_SAVE_HPP_ */
