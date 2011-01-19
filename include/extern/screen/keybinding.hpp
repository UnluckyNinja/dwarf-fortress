/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_SCREEN_KEYBINDING_HPP_
#define EXTERN_SCREEN_KEYBINDING_HPP_

#include <string>
#include <vector>

#include "hidden/viewscreen.hpp"

#include "intern/enabler/input/event_match.hpp"

#include "intern/screen/widget/menu.hpp"

struct BindingGroup {
    BindingGroup(const ::std::string& name_in, const interface_key_t start_in, const interface_key_t end_in);

    ::std::string name;
    interface_key_t start;
    interface_key_t end;
};

class KeybindingScreen: public viewscreenst {
    static ::std::vector< BindingGroup > groups;

    enum {
      mode_main, mode_keyL, mode_keyR, mode_macro, mode_register
    } mode;
    enum keyR_type {
      sel_add, sel_rep_none, sel_rep_slow, sel_rep_fast, sel_event
    };
    enum main_selector {
      sel_macros, sel_just_exit, sel_save_exit, sel_first_group
    };

    struct keyR_selector {
        keyR_type sel;
        event_match event; // Uninitialized if sel != sel_event
    };

    ::widgets::menu< int > main; // Representing main_selector
    ::widgets::menu< interface_key_t > keyL;
    ::widgets::menu< keyR_selector > keyR;
    ::widgets::menu< ::std::string > macro;
    ::widgets::menu< match_type > keyRegister;

    void render_main();
    void render_macro();
    void render_key();
    void render_register();

    void reset_keyR();

    void enter_key(int group);
    void enter_macros();

  public:
    /**
     * @ingroup extern
     */
    KeybindingScreen();
    virtual void feed(::std::set< interface_key_t > &events);
    virtual void render();
    virtual void help();
    virtual void logic();
    virtual char is_option_screen();
};

#endif /* EXTERN_SCREEN_KEYBINDING_HPP_ */
