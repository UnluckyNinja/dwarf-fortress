/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#define BOOST_PARAMETER_MAX_ARITY 7

#include <sstream>
#include <set>

#include "extern/screen/keybinding.hpp"

#include "hidden/global.hpp"
#include "hidden/dwarf.hpp"

#include "extern/init.hpp"
#include "extern/interface.hpp"
#include "extern/enabler.hpp"
#include "intern/enabler/input.hpp"

#include <logging/logging.hpp>

#include "intern/keybindings.hpp"

// Render the main menu
void KeybindingScreen::render_main() {
  drawborder("Key binding & macro center");
  main.render(6, init.display.grid_x - 3, 3, init.display.grid_y - 4);
}

void KeybindingScreen::render_macro() {
  drawborder("Macros");
  gps.locate(3, 3);
  gps.changecolor(4, 0, 1);
  gps.addst("Select a macro, then press " + enabler.GetKeyDisplay(INTERFACEKEY_STRING_A000) + " to delete.");
  macro.render(6, init.display.grid_x - 2, 5, init.display.grid_y - 2);
}

void KeybindingScreen::render_key() {
  if (enabler_input::instance().is_registering()) {
    gps.changecolor(4, 0, 1);
    drawborder("Keybinding - currently registering new key");
  } else
    drawborder("Keybinding");
  gps.locate(3, 6);
  gps.changecolor(4, 0, 1);
  gps.addst("Select a binding, then press " + enabler.GetKeyDisplay(INTERFACEKEY_STRING_A000) + " to delete.");
  keyL.render(6, init.display.grid_x / 2 - 1, 5, init.display.grid_y - 2);
  if (mode == mode_keyL || mode == mode_register)
    keyR.bleach(true);
  else
    keyR.bleach(false);
  keyR.render(init.display.grid_x / 2 + 1, init.display.grid_x - 2, 5, init.display.grid_y - 2);
}

void KeybindingScreen::render_register() {
  int x1 = init.display.grid_x / 2 - 20, x2 = init.display.grid_x / 2 + 20, y1 = init.display.grid_y / 2 - 1, y2 =
      init.display.grid_y / 2 + 1;
  if (!enabler_input::instance().is_registering()) {
    y2 = y1 + keyRegister.size() + 1;
  }
  gps.erasescreen_rect(x1, x2, y1, y2);
  gps.changecolor(1, 1, 1);
  for (int x = x1; x <= x2; x++) {
    gps.locate(y1, x);
    gps.addchar(' ');
    gps.locate(y2, x);
    gps.addchar(' ');
  }
  for (int y = y1 + 1; y < y2; y++) {
    gps.locate(y, x1);
    gps.addchar(' ');
    gps.locate(y, x2);
    gps.addchar(' ');
  }
  if (enabler_input::instance().is_registering()) {
    gps.changecolor(7, 0, 1);
    gps.locate(y1 + 1, x1 + 2);

    ::std::stringstream str;
    str << enabler_input::instance().get_mod_state();

    gps.addst(str.str());
  } else {
    keyRegister.render(x1 + 1, x2 - 1, y1 + 1, y2 - 1);
    gps.locate(y2, x1 + 2);
    gps.changecolor(7, 1, 1);
    gps.addst("Select binding, or press space to abort");
  }
}

void KeybindingScreen::reset_keyR() {
  int lastpos = keyR.get_pos();
  keyR.clear();
  struct keyR_selector sel;
  sel.sel = sel_add;
  keyR.add("Add binding", sel);
  interface_key_t key = keyL.get_selection();
  ::std::list< event_match > matchers = enabler_input::instance().list_keys(key);
  repeat_speed rep = enabler_input::instance().get_key_repeat(key);
  sel.sel = sel_rep_none;
  keyR.set(2, "Don't repeat", sel);
  if (rep == REPEAT_NOT)
    keyR.set_color(2, 4, 0);
  sel.sel = sel_rep_slow;
  keyR.set(3, "Delayed repeat", sel);
  if (rep == REPEAT_SLOW)
    keyR.set_color(3, 4, 0);
  sel.sel = sel_rep_fast;
  keyR.set(4, "Immediate repeat", sel);
  if (rep == REPEAT_FAST)
    keyR.set_color(4, 4, 0);
  int i = 6;
  for (::std::list< event_match >::iterator it = matchers.begin(); it != matchers.end(); ++it, ++i) {
    ::std::ostringstream desc;
    switch (it->type) {
      case type_unicode:
        desc << "By letter: ";
        if (it->unicode < 256 && isgraph(it->unicode)) // Is it printable?
          desc << (char) it->unicode;
        else
          desc << "U+" << ::std::hex << ::std::uppercase << it->unicode;
        break;
      case type_key:
        desc << "By position: " << it->mod << sdlNames.left.at(it->key);
        break;
      case type_button:
        desc << "Mouse: " << (int) it->button;
        break;
    }
    sel.sel = sel_event;
    sel.event = *it;
    keyR.set(i, desc.str(), sel);
  }
  keyR.set_pos(lastpos);
}

void KeybindingScreen::enter_key(int group) {
  mode = mode_keyL;
  keyL.clear();
  for (interface_key_t i = groups[group].start; i <= groups[group].end; i++) {
    if (i != INTERFACEKEY_NONE)
      keyL.add(enabler_input::instance().GetBindingTextDisplay(i), i);
  }
  reset_keyR();
}

void KeybindingScreen::enter_macros() {
  mode = mode_macro;
  macro.clear();
  ::std::list< ::std::string > macros = enabler_input::instance().list_macros();
  for (::std::list< ::std::string >::iterator it = macros.begin(); it != macros.end(); ++it)
    macro.add(*it, *it);
  if (!macros.size())
    macro.add("No macros!", "");
}

void KeybindingScreen::feed(::std::set< interface_key_t > &input) {
  enabler.flag |= ENABLERFLAG_RENDER;
  if (input.count(INTERFACEKEY_KEYBINDING_COMPLETE)) {
    ::std::list< registered_key > keys = enabler_input::instance().getRegisteredKey();
    if (keys.size() == 0) {
      __info
        << "No keys registered ?!";
      mode = mode_keyR;
    } else {
      keyRegister.clear();
      ::std::list< registered_key > keys = enabler_input::instance().getRegisteredKey();
      for (::std::list< registered_key >::iterator it = keys.begin(); it != keys.end(); ++it) {
        ::std::string display;
        switch (it->type) {
          case type_button:
            display = "Mouse button: ";
            break;
          case type_key:
            display = "By position: ";
            break;
          case type_unicode:
            display = "By letter: ";
            break;
        }
        keyRegister.add(display + it->display, it->type);
      }
    }
  } else if (input.count(INTERFACEKEY_STANDARDSCROLL_PAGEUP) || input.count(INTERFACEKEY_STANDARDSCROLL_PAGEDOWN)
      || input.count(INTERFACEKEY_STANDARDSCROLL_UP) || input.count(INTERFACEKEY_STANDARDSCROLL_DOWN)) {
    switch (mode) {
      case mode_main:
        main.feed(input);
        break;
      case mode_keyL:
        keyL.feed(input);
        reset_keyR();
        break;
      case mode_keyR:
        keyR.feed(input);
        break;
      case mode_macro:
        macro.feed(input);
        break;
      case mode_register:
        keyRegister.feed(input);
        break;
    }
  } else if (mode == mode_keyL && input.count(INTERFACEKEY_STANDARDSCROLL_RIGHT))
    mode = mode_keyR;
  else if (mode == mode_main && input.count(INTERFACEKEY_STANDARDSCROLL_RIGHT)) {
    if (main.get_selection() == sel_macros)
      enter_macros();
    if (main.get_selection() >= sel_first_group)
      enter_key(main.get_selection() - sel_first_group);
  } else if (mode == mode_keyR && input.count(INTERFACEKEY_STANDARDSCROLL_LEFT))
    mode = mode_keyL;
  else if ((mode == mode_keyL || mode == mode_macro) && input.count(INTERFACEKEY_STANDARDSCROLL_LEFT))
    mode = mode_main;
  else if (input.count(INTERFACEKEY_STRING_A000)) { // Backspace: Delete something.
    keyR_selector sel;
    switch (mode) {
      case mode_macro:
        if (macro.get_selection() != "") {
          enabler_input::instance().delete_macro(macro.get_selection());
          macro.del_selection();
          if (!macro.size())
            macro.add("No macros!", "");
        }
        break;
      case mode_keyR:
        sel = keyR.get_selection();
        if (sel.sel == sel_event) {
          enabler_input::instance().remove_key(keyL.get_selection(), sel.event);
          reset_keyR();
        }
        break;
      case mode_keyL:
      case mode_main:
      case mode_register:
        break;
    }
  } else if (input.count(INTERFACEKEY_SELECT)) {
    switch (mode) {
      case mode_main:
        if (main.get_selection() == sel_macros) { // Macros
          enter_macros();
        } else if (main.get_selection() == sel_save_exit) { // Save and exit
          enabler_input::instance().save_keybindings();
          breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
          return;
        } else if (main.get_selection() == sel_just_exit) { // Just exit
          breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
          return;
        } else { // Some key-binding group
          enter_key(main.get_selection() - sel_first_group);
        }
        break;
      case mode_keyR: {
        interface_key_t key = keyL.get_selection();
        switch (keyR.get_selection().sel) {
          case sel_add:
            enabler_input::instance().register_key();
            mode = mode_register;
            break;
          case sel_rep_none:
            enabler_input::instance().set_key_repeat(key, REPEAT_NOT);
            reset_keyR();
            break;
          case sel_rep_slow:
            enabler_input::instance().set_key_repeat(key, REPEAT_SLOW);
            reset_keyR();
            break;
          case sel_rep_fast:
            enabler_input::instance().set_key_repeat(key, REPEAT_FAST);
            reset_keyR();
            break;
          default:
            break;
        }
      }
        break;
      case mode_register:
        enabler_input::instance().bindRegisteredKey(keyRegister.get_selection(), keyL.get_selection());
        mode = mode_keyR;
        reset_keyR();
        break;
      default:
        break;
    }
  } else if (input.count(INTERFACEKEY_LEAVESCREEN) || input.count(INTERFACEKEY_OPTIONS)) {
    if (mode == mode_register)
      mode = mode_keyR;
    else
      mode = mode_main;
  }
}

void KeybindingScreen::render() {
  switch (mode) {
    case mode_main:
      render_main();
      break;
    case mode_keyL:
    case mode_keyR:
      render_key();
      break;
    case mode_macro:
      render_macro();
      break;
    case mode_register:
      render_key();
      render_register();
      break;
  }
}

void KeybindingScreen::help() {
}

void KeybindingScreen::logic() {
  if (mode == mode_register)
    enabler.flag |= ENABLERFLAG_RENDER;
}

char KeybindingScreen::is_option_screen() {
  return 2;
}
