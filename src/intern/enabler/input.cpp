/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include "intern/enabler/input.hpp"

#include <fstream>
#include <boost/regex.hpp>
#include <boost/cast.hpp>

#include "hidden/global.hpp"
#include "extern/init.hpp"

#include "intern/utils/filesystem.hpp"
#include "intern/utils/numbers.hpp"
#include <logging/logging.hpp>

#include "intern/events/event_engine.hpp"

#include "intern/keybindings.hpp"

#include <SDL/SDL_timer.h>

static ::std::string display(const event_match& match) {
  ::std::ostringstream ret;
  ret << match.mod;

  switch (match.type) {
    case type_unicode:
      ret << (char) match.unicode;
      break;
    case type_key: {
      sdl_names_t::left_iterator it = sdlNames.left.find(match.key);
      if (it != sdlNames.left.end()) {
        ret << it->second;
      } else {
        ret << "SDL+" << (int) match.key;
      }

      break;
    }
    case type_button:
      ret << "Button " << (int) match.button;
      break;
  }
  return ret.str();
}

static ::std::string translate_repeat(repeat_speed r) {
  switch (r) {
    case REPEAT_NOT:
      return "REPEAT_NOT";
    case REPEAT_SLOW:
      return "REPEAT_SLOW";
    case REPEAT_FAST:
      return "REPEAT_FAST";
    default:
      return "REPEAT_BROKEN";
  }
}

// Replaces any illegal letters.
static ::std::string filter_filename(::std::string name, char replacement) {
  for (::std::size_t i = 0; i < name.length(); i++) {
    switch (name[i]) {
      case '<':
      case '>':
      case ':':
      case '"':
      case '/':
      case '\\':
      case '|':
      case '?':
      case '*':
        name[i] = replacement;
        break;
      default:
        if (::std::iscntrl(name[i])) {
          name[i] = replacement;
        }
        break;
    }
  }

  return name;
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

enabler_input::enabler_input() {
  last_serial = 0;
  macro_recording = false;
  key_registering = false;
  macro_end = 0;

  event_engine& engine = event_engine::instance();
  engine.on_event< key_event > (::boost::bind(&enabler_input::on_key_event, this, _1));
  engine.on_event< interface_event > (::boost::bind(&enabler_input::on_interface_event, this, _1));
}

enabler_input::keydisplay_t& enabler_input::get_keydisplay() {
  return keydisplay;
}

// Returns an unused serial number
int32_t enabler_input::next_serial() {
  return ++last_serial;
}

// Converts SDL mod states to ours, collapsing left/right shift/alt/ctrl
mod_state& enabler_input::get_mod_state() {
  return state;
}

void enabler_input::update_keydisplay(interface_key_t binding, ::std::string display) {
  // Need to filter out space/tab, for obvious reasons.
  if (display == " ") {
    display = "Space";
  }

  if (display == "\t") {
    display = "Tab";
  }

  keydisplay_t::iterator it = keydisplay.find(binding);

  if (it == keydisplay.end()) {
    string_set_by_len_t s;
    s.insert(display);
    keydisplay[binding] = s;
  } else {
    keydisplay[binding].insert(display);
  }
}

::std::set< interface_key_t > enabler_input::key_translation(const event_match& match) {
  typedef keymap_t::iterator iterator_t;

  ::std::set< interface_key_t > bindings;
  ::std::pair< iterator_t, iterator_t > its;

  its = keymap.equal_range(match);
  for (; its.first != its.second; ++its.first) {
    bindings.insert(its.first->second);
  }

  return bindings;
}

repeat_speed enabler_input::get_key_repeat(interface_key_t binding) {
  ::std::map< interface_key_t, repeat_speed >::iterator it = repeatmap.find(binding);

  if (it != repeatmap.end()) {
    return it->second;
  } else {
    return REPEAT_NOT;
  }
}

void enabler_input::set_key_repeat(interface_key_t binding, repeat_speed repeat) {
  repeatmap[binding] = repeat;
}

void enabler_input::load_macro_from_file(const ::std::string &file) {
  ::std::ifstream s(file.c_str());
  char buf[100];
  s.getline(buf, 100);
  ::std::string name(buf);
  if (macros.find(name) != macros.end())
    return; // Already got it.

  interface_key_set_list_t macro;
  interface_key_set_t group;
  for (;;) {
    s.getline(buf, 100);
    if (!s.good()) {
      __error
        << "I/O error while loading macro";
      s.close();
      return;
    }
    ::std::string line(buf);
    if (line == "End of macro") {
      if (group.size())
        macro.push_back(group);
      break;
    } else if (line == "\tEnd of group") {
      if (group.size())
        macro.push_back(group);
      group.clear();
    } else if (line.substr(0, 2) != "\t\t") {
      if (line.substr(1).find("\t") != ::std::string::npos) {
        // expecting /t##/tCMD for a repeated command
        ::std::istringstream ss(line.substr(1));
        int count;
        ::std::string remainingline;

        if (ss >> count) {
          ss >> remainingline;
          if (remainingline.size()) {
            for (int i = 0; i < count; i++) {
              ::boost::bimap< interface_key_t, ::std::string >::right_iterator it = bindingNames.right.find(remainingline);
              if (it == bindingNames.right.end()) {
                __info
                  << "Binding name unknown while loading macro: " << line.substr(1);
              } else {
                group.insert(it->second);
                if (group.size())
                  macro.push_back(group);
                group.clear();
              }
            }
          } else {
            __info
              << "Binding missing while loading macro: " << line.substr(1);
          }
        } else {
          __info
            << "Quantity not numeric or Unexpected tab(s) while loading macro: " << line.substr(1);
        }
      } else {
        // expecting /tCMD for a non-grouped command
        ::boost::bimap< interface_key_t, ::std::string >::right_iterator it = bindingNames.right.find(line.substr(1));
        if (it == bindingNames.right.end()) {
          __info
            << "Binding name unknown while loading macro: " << line.substr(1);
        } else {
          group.insert(it->second);
          if (group.size())
            macro.push_back(group);
          group.clear();
        }
      }
    } else {
      ::boost::bimap< interface_key_t, ::std::string >::right_iterator it = bindingNames.right.find(line.substr(2));
      if (it == bindingNames.right.end())
        __info
          << "Binding name unknown while loading macro: " << line.substr(2);
      else
        group.insert(it->second);
    }
  }
  if (s.good())
    macros[name] = macro;
else    __error << "I/O error while loading macro";
    s.close();
  }

void enabler_input::save_macro_to_file(const ::std::string &file, const ::std::string &name, const interface_key_set_list_t &macro) {
  ::std::ofstream s(file.c_str());
  s << name << ::std::endl;
  for (interface_key_set_list_t::const_iterator group = macro.begin(); group != macro.end(); ++group) {
    for (interface_key_set_t::const_iterator key = group->begin(); key != group->end(); ++key)
      s << "\t\t" << bindingNames.left.at(*key) << ::std::endl;
    s << "\tEnd of group" << ::std::endl;
  }
  s << "End of macro" << ::std::endl;
  s.close();
}

void enabler_input::add_input(sdl_event_t &e, uint32_t now) {
  event_engine& engine = event_engine::instance();

  // Before we can use this input, there are some issues to deal with:
  // - SDL provides unicode translations only for key-press events, not
  //   releases. We need to keep track of pressed keys, and generate
  //   unicode release events whenever any modifiers are hit, or if
  //   that raw keycode is released.
  // - Generally speaking, when modifiers are hit/released, we discard those
  //   events and generate press/release events for all pressed non-modifiers.
  // - It's possible for multiple events to be generated on the same tick.
  //   These are of course separate keypresses, and must be kept separate.
  //   That's what the serial is for.
  ::boost::unique_lock< ::boost::mutex > lock(pressed_keys_mutex);
  event_match_set_t current_pressed = pressed_keys;
  lock.unlock();

  ::std::set< event_match >::iterator pkit;

  // If mod state has changed
  if (get_mod_state().update(e.key)) {
    // Convert modifier state changes
    for (pkit = current_pressed.begin(); pkit != current_pressed.end(); ++pkit) {
      // Release currently pressed keys
      event< key_event > eve;
      eve.release = true;
      eve.match = *pkit;
      engine.fire(eve);

      // Re-press them, with new modifiers, if they aren't unicode. We can't re-translate unicode.
      if (eve.match.type != type_unicode) {
        // We don't want extras when registering keys
        if (!key_registering) {
          eve.release = false;
          eve.match.mod = get_mod_state();
          engine.fire(eve);
        }
      }
    }
  } else {
    // It's not a modifier. If this is a key release, then we still need
    // to find and release pressed unicode keys with this scancode
    if (e.type == SDL_KEYUP) {
      for (pkit = current_pressed.begin(); pkit != current_pressed.end(); ++pkit) {
        if (pkit->type == type_unicode && pkit->scancode == e.key.keysym.scancode) {
          event< key_event > eve;
          eve.release = true;
          eve.match = *pkit;
          engine.fire(eve);
        }
      }
    }
    // Since it's not a modifier, we also pass on symbolic/button
    // (always) and unicode (if defined) events

    // However, since SDL ignores(?) ctrl and alt when translating to
    // unicode, we want to ignore unicode events if those are set.
    event< key_event > eve;
    eve.release = (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP) ? true : false;
    eve.match.mod = get_mod_state();

    key_event real;
    real.release = (e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONUP) ? true : false;
    real.match.mod = get_mod_state();
    if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN) {
      eve.match.type = type_button;
      eve.match.scancode = 0;
      eve.match.button = e.button.button;
      engine.fire(eve);
    }

    if (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) {
      eve.match.type = type_key;
      eve.match.scancode = e.key.keysym.scancode;
      eve.match.key = e.key.keysym.sym;
      engine.fire(eve);
    }

    if (e.type == SDL_KEYDOWN && e.key.keysym.unicode && get_mod_state() < 2) {
      eve.match.mod = KMOD_NONE;
      eve.match.type = type_unicode;
      eve.match.scancode = e.key.keysym.scancode;
      eve.match.unicode = e.key.keysym.unicode;
      engine.fire(eve);
    }

    if (e.type == SDL_QUIT) {
      // This one, we insert directly into the timeline.
      event< interface_event > int_eve;
      int_eve.r = REPEAT_NOT;
      int_eve.k = INTERFACEKEY_OPTIONS;
      int_eve.repeats = 0;

      engine.fire(int_eve);
    }
  }
}

bool cancel_if_has_repeated(event< interface_event >& e) {
  if (e.repeats > 0) {
    e.cancel();
  } else {
    e.r = REPEAT_NOT;
  }

  return false;
}

void enabler_input::on_key_event(const event< key_event >& e) {
  // Add or remove the key from pressed_keys, keeping that up to date
  ::boost::unique_lock< ::boost::mutex > pressed_lock(pressed_keys_mutex);
  if (e.release) {
    pressed_keys.erase(e.match);
  } else {
    pressed_keys.insert(e.match);
  }
  pressed_lock.unlock();

  event_engine& engine = event_engine::instance();

  // We may be registering a new mapping, in which case we skip the
  // rest of this function.
  if (key_registering && !e.release) {
    stored_keys.push_back(e.match);

    event< interface_event > int_eve;
    int_eve.r = REPEAT_NOT;
    int_eve.repeats = 0;
    int_eve.k = INTERFACEKEY_KEYBINDING_COMPLETE;

    engine.fire(int_eve);
    return;
  }

  // If this is a key-press event, we add it to the timeline. If it's
  // a release, we remove any pending repeats, but not those that
  // haven't repeated yet (which are on their first cycle); those we
  // just set to non-repeating.
  ::std::set< interface_key_t > keys = key_translation(e.match);
  if (e.release) {
    ::boost::lock_guard< ::boost::mutex > timeline_lock(timeline_mutex);

    event_engine::instance().transform_future< interface_event > (cancel_if_has_repeated, e.get_datetime());
  } else {
    ::std::set< interface_key_t >::iterator key;
    // As policy, when the user hits a non-repeating key we'd want to
    // also cancel any keys that are currently repeating. This allows
    // for easy recovery from stuck keys.
    //
    // Unfortunately, each key may be bound to multiple
    // commands. So, lacking information on which commands are
    // accepted at the moment, there is no way we can know if it's
    // okay to cancel repeats unless /all/ the bindings are
    // non-repeating.
    for (::std::set< interface_key_t >::iterator k = keys.begin(); k != keys.end(); ++k) {
      event< interface_event > int_eve;
      int_eve.r = get_key_repeat(*k);
      int_eve.k = *k;
      int_eve.repeats = 0; // Starts at 0, increments once per repeat

      engine.fire(int_eve);
    }
    // if (cancel_ok) {
    //   // Set everything on the timeline to non-repeating
    //   multimap<Time,Event>::iterator it;
    //   for (it = timeline.begin(); it != timeline.end(); ++it) {
    //     it->second.r = REPEAT_NOT;
    //   }
  }
}

void enabler_input::on_interface_event(const event< interface_event >& e) {
  // Add or remove the key from pressed_keys, keeping that up to date
  ::boost::lock_guard< ::boost::mutex > timeline_lock(timeline_mutex);

  timeline.push_back(e);

  // Schedule a repeat
  ::boost::posix_time::time_duration delay;
  switch (e.r) {
    case REPEAT_NOT:
      return;
    case REPEAT_SLOW:
      delay = ::boost::posix_time::milliseconds(init.input.hold_time);
      break;
    case REPEAT_FAST:
      double accel = 1;
      // Compute acceleration
      if (e.repeats >= init.input.repeat_accel_start) {
        accel = ::std::min(double(init.input.repeat_accel_limit), sqrt(double(e.repeats + 1 - init.input.repeat_accel_start) + 16) - 3);
      }
      int millis = ::std::max(int(double(init.input.repeat_time) / accel), 25);
      delay = ::boost::posix_time::milliseconds(millis);
      break;
  }

  event< interface_event > next(e, delay);
  next.repeats++;

  event_engine::instance().fire(next);
}

// Input encoding:
// 1 and up are ncurses symbols, as returned by getch.
// -1 and down are unicode values.
// esc is true if this key was part of an escape sequence.
#ifdef CURSES
void enabler_input::add_input_ncurses(int key, uint32_t now, bool esc) {
  // TODO: Deal with shifted arrow keys, etc. See man 5 terminfo and tgetent.

  event_match sdl, uni; // Each key may provoke an unicode event, an "SDL-key" event, or both
  const int serial = next_serial();
  sdl.type = type_key;
  uni.type = type_unicode;
  sdl.scancode = uni.scancode = 0; // We don't use this.. hang on, who does? ..nobody. FIXME!
  sdl.mod = uni.mod = 0;
  sdl.key = SDLK_UNKNOWN;
  uni.unicode = 0;

  if (esc) { // Escape sequence, meaning alt was held. I hope.
    sdl.mod = uni.mod = mod_state(mod_state::MOD_ALT);
  }

  if (key == -10) { // Return
    sdl.key = SDLK_RETURN;
    uni.unicode = '\n';
  } else if (key == -9) { // Tab
    sdl.key = SDLK_TAB;
    uni.unicode = '\t';
  } else if (key == -27) { // If we see esc here, it's the actual esc key. Hopefully.
    sdl.key = SDLK_ESCAPE;
  } else if (key == -127) { // Backspace/del
    sdl.key = SDLK_BACKSPACE;
  } else if (key < 0 && key >= -26) { // Control-a through z (but not ctrl-j, or ctrl-i)
    sdl.mod |= DFMOD_CTRL;
    sdl.key = (sdl_key_t) (SDLK_a + (-key) - 1);
  } else if (key <= -32 && key >= -126) { // ASCII character set
    uni.unicode = -key;
    sdl.key = (sdl_key_t) -key; // Most of this maps directly to SDL keys, except..
    if (sdl.key > 64 && sdl.key < 91) { // Uppercase
      sdl.key = (sdl_key_t) (sdl.key + 32); // Maps to lowercase, and
      sdl.mod |= DFMOD_SHIFT; // Add shift.
    }
  } else if (key < -127) { // Unicode, no matching SDL keys
    uni.unicode = -key;
  } else if (key > 0) { // Symbols such as arrow-keys, etc, no matching unicode.
    switch (key) {
      case KEY_DOWN:
      sdl.key = SDLK_DOWN;
      break;
      case KEY_UP:
      sdl.key = SDLK_UP;
      break;
      case KEY_LEFT:
      sdl.key = SDLK_LEFT;
      break;
      case KEY_RIGHT:
      sdl.key = SDLK_RIGHT;
      break;
      case KEY_BACKSPACE:
      sdl.key = SDLK_BACKSPACE;
      break;
      case KEY_F(1):
      sdl.key = SDLK_F1;
      break;
      case KEY_F(2):
      sdl.key = SDLK_F2;
      break;
      case KEY_F(3):
      sdl.key = SDLK_F3;
      break;
      case KEY_F(4):
      sdl.key = SDLK_F4;
      break;
      case KEY_F(5):
      sdl.key = SDLK_F5;
      break;
      case KEY_F(6):
      sdl.key = SDLK_F6;
      break;
      case KEY_F(7):
      sdl.key = SDLK_F7;
      break;
      case KEY_F(8):
      sdl.key = SDLK_F8;
      break;
      case KEY_F(9):
      sdl.key = SDLK_F9;
      break;
      case KEY_F(10):
      sdl.key = SDLK_F10;
      break;
      case KEY_F(11):
      sdl.key = SDLK_F11;
      break;
      case KEY_F(12):
      sdl.key = SDLK_F12;
      break;
      case KEY_F(13):
      sdl.key = SDLK_F13;
      break;
      case KEY_F(14):
      sdl.key = SDLK_F14;
      break;
      case KEY_F(15):
      sdl.key = SDLK_F15;
      break;
      case KEY_DC:
      sdl.key = SDLK_DELETE;
      break;
      case KEY_NPAGE:
      sdl.key = SDLK_PAGEDOWN;
      break;
      case KEY_PPAGE:
      sdl.key = SDLK_PAGEUP;
      break;
      case KEY_ENTER:
      sdl.key = SDLK_RETURN;
      break;
    }
  }

  // We may be registering a new mapping, in which case we skip the
  // rest of this function.
  if (key_registering) {
    if (uni.unicode) {
      stored_keys.push_back(uni);
    }
    if (sdl.key) {
      stored_keys.push_back(sdl);
    }
    event< interface_event > e;
    e.r = REPEAT_NOT;
    e.repeats = 0;
    e.time = ::boost::get_system_time();
    e.serial = serial;
    e.k = INTERFACEKEY_KEYBINDING_COMPLETE;
    e.tick = enabler.get_simticks();

    event_engine::instance().fire(e);

    key_registering = false;
    return;
  }

  // Key repeat is handled by the terminal, and we don't get release
  // events anyway.
  key_event kev;
  kev.release = false;
  event< interface_event > e;
  e.r = REPEAT_NOT;
  e.repeats = 0;
  e.time = ::boost::get_system_time();
  if (sdl.key) {
    ::std::set< interface_key_t > events = key_translation(sdl);
    for (::std::set< interface_key_t >::iterator k = events.begin(); k != events.end(); ++k) {
      e.serial = serial;
      e.k = *k;

      event_engine::instance().fire(e);
    }
  }
  if (uni.unicode) {
    ::std::set< interface_key_t > events = key_translation(uni);
    for (::std::set< interface_key_t >::iterator k = events.begin(); k != events.end(); ++k) {
      e.serial = serial;
      e.k = *k;

      event_engine::instance().fire(e);
    }
  }
}
#endif

interface_key_set_t enabler_input::get_input(::boost::system_time now) {

  // We walk the timeline, returning all events corresponding to a
  // single physical keypress, and inserting repeats relative to the
  // current time, not when the events we're now returning were
  // *supposed* to happen.

  interface_key_set_t input;

  ::boost::unique_lock< ::boost::mutex > timeline_lock(timeline_mutex);
  timeline_t events(timeline.begin(), timeline.end());
  timeline.clear();
  timeline_lock.unlock();

  timeline_t::iterator it = events.begin();
  for (; it != events.end(); ++it) {
    input.insert(it->k);
  }

  //  const ::boost::system_time first_time = ev->time;
  //  const int first_serial = ev->serial;
  //
  //  int simtick = enabler.get_simticks();
  //  while (ev != timeline.end() && ev->time == first_time && ev->serial == first_serial) {
  //    // To make sure the user had a chance to cancel (by lifting the key), we require there
  //    // to be at least three simulation ticks before the first repeat.
  //    if (ev->repeats == 1 && ev->tick > simtick - 3) {
  //    } else {
  //      input.insert(ev->k);
  //    }
  //
  //    // Schedule a repeat
  //    event2< event > next;
  //    next.k = ev->k;
  //    next.r = ev->r;
  //    next.repeats = ev->repeats + 1;
  //    next.serial = ev->serial;
  //    next.tick = ev->tick;
  //    next.time = ev->time;
  //    switch (next.r) {
  //      case REPEAT_NOT:
  //        break;
  //      case REPEAT_SLOW:
  //        if (ev->repeats == 0) {
  //          next.time = now + ::boost::posix_time::milliseconds(init.input.hold_time);
  //
  //          event_engine::instance().fire(next);
  //          break;
  //        }
  //      case REPEAT_FAST:
  //        double accel = 1;
  //        if (ev->repeats >= init.input.repeat_accel_start) {
  //          // Compute acceleration
  //          accel = MIN(init.input.repeat_accel_limit, sqrt(double(next.repeats - init.input.repeat_accel_start) + 16)
  //              - 3);
  //        }
  //        next.time = now + ::boost::posix_time::milliseconds(double(init.input.repeat_time) / accel);
  //
  //        event_engine::instance().fire(next);
  //        break;
  //    }
  //    // Delete the event from the timeline and iterate
  //    timeline.erase(ev++);
  //  }

#ifdef DEBUG
  if (input.size() && !init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    __info
    << "Returning input:";
    interface_key_set_t::iterator it;
    for (it = input.begin(); it != input.end(); ++it) {
      __info
      << "    " << GetKeyDisplay(*it) << ": " << GetBindingDisplay(*it);
    }
  }
#endif
  // It could be argued that the "record event" step of recording
  // belongs in add_input, not here.  I don't hold with this
  // argument. The whole point is to record events as the user seems
  // them happen.
  if (macro_recording) {
    interface_key_set_t macro_input = input;

    macro_input.erase(INTERFACEKEY_RECORD_MACRO);
    macro_input.erase(INTERFACEKEY_PLAY_MACRO);
    macro_input.erase(INTERFACEKEY_SAVE_MACRO);
    macro_input.erase(INTERFACEKEY_LOAD_MACRO);

    if (macro_input.size()) {
      active_macro.push_back(macro_input);
    }
  }

  return input;
}

void enabler_input::clear_input() {
  ::boost::lock_guard< ::boost::mutex > timeline_lock(timeline_mutex);
  ::boost::lock_guard< ::boost::mutex > pressed_lock(pressed_keys_mutex);

  timeline.clear();
  pressed_keys.clear();
  enabler_input::get_mod_state().clear();
  last_serial = 0;
}

void enabler_input::load_keybindings(const ::std::string &file) {
  __info
    << "Loading bindings from " << file;
  interfacefile = file;
  ::std::ifstream s(file.c_str());
  if (!s.good()) {
    __fatal
      << file << " not found, or I/O error encountered";
    ::std::exit(666);
  }

  ::std::list< ::std::string > lines;
  while (s.good()) {
    ::std::string line;
    ::std::getline(s, line);
    lines.push_back(line);
  }
  s.close();

  ::boost::regex bind_expr("\\[BIND:([^:\\]]+):([^:\\]]+)\\]");
  ::boost::regex sym_expr("\\[SYM:([^:\\]]+):([^:\\]]+)\\]");
  ::boost::regex key_expr("\\[KEY:([^:\\]]+)\\]");
  ::boost::regex button_expr("\\[BUTTON:([^:\\]]+):([^:\\]]+)\\]");
  ::boost::sregex_iterator end;

  ::std::list< ::std::string >::iterator line = lines.begin();
  ::std::vector< ::std::string > match;

  while (line != lines.end()) {
    ::boost::sregex_iterator bind_it((*line).begin(), (*line).end(), bind_expr);

    if (bind_it != end) {
      ::std::string binding_name = bind_it->str(1);
      ::std::string repeat_name = bind_it->str(2);

      ::boost::bimap< interface_key_t, ::std::string >::right_iterator it = bindingNames.right.find(binding_name);
      if (it != bindingNames.right.end()) {
        interface_key_t binding = it->second;
        // Parse repeat data
        if (repeat_name.compare("REPEAT_FAST") == 0) {
          repeatmap[(interface_key_t) binding] = REPEAT_FAST;
        } else if (repeat_name.compare("REPEAT_SLOW") == 0) {
          repeatmap[(interface_key_t) binding] = REPEAT_SLOW;
        } else if (repeat_name.compare("REPEAT_NOT") == 0) {
          repeatmap[(interface_key_t) binding] = REPEAT_NOT;
        } else {
          repeatmap[(interface_key_t) binding] = REPEAT_NOT;
          __info
            << "Broken repeat request: " << repeat_name;
        }

        ++line;
        // Add symbols/keys/buttons
        while (line != lines.end()) {
          event_match matcher;

          // SDL Keys
          ::boost::sregex_iterator sym_it((*line).begin(), (*line).end(), sym_expr);
          ::boost::sregex_iterator key_it((*line).begin(), (*line).end(), key_expr);
          ::boost::sregex_iterator button_it((*line).begin(), (*line).end(), button_expr);
          if (sym_it != end) {
            ::std::string mod_code = sym_it->str(1);
            ::std::string sdl_name = sym_it->str(2);

            ::boost::bimap< sdl_key_t, ::std::string >::right_iterator it = sdlNames.right.find(sdl_name);
            if (it != sdlNames.right.end()) {
              matcher.mod = to_uint32(mod_code);
              matcher.type = type_key;
              matcher.key = it->second;
              keymap.insert(::std::pair< event_match, interface_key_t >(matcher, (interface_key_t) binding));
              update_keydisplay(binding, display(matcher));
            } else {
              __info
                << "Unknown SDLKey: " << sdl_name;
            }
            ++line;

            // Unicode
          } else if (key_it != end) {
            ::std::string unicode = key_it->str(1);

            matcher.type = type_unicode;
            matcher.unicode = decode_utf8(unicode);
            matcher.mod = KMOD_NONE;
            if (matcher.unicode) {
              keymap.insert(::std::make_pair(matcher, (interface_key_t) binding));
              if (matcher.unicode < 256) {
                // This unicode key is part of the latin-1 mapped portion of unicode, so we can
                // actually display it. Nice.
                char c[2] = { char(matcher.unicode), 0 };
                update_keydisplay(binding, display(matcher));
              }
            } else {
              __info
                << "Broken unicode: " << *line;
            }
            ++line;

            // Mouse buttons
          } else if (button_it != end) {
            ::std::string mod_code = button_it->str(1);
            ::std::string button_code = button_it->str(2);

            matcher.type = type_button;
            matcher.button = to_uint32(button_code);
            if (matcher.button) {
              matcher.mod = to_uint32(mod_code);
              keymap.insert(::std::pair< event_match, interface_key_t >(matcher, (interface_key_t) binding));
              update_keydisplay(binding, display(matcher));
            } else {
              __info
                << "Broken button (should be [BUTTON:#:#]): " << *line;
            }
            ++line;
          } else {
            break;
          }
        }
      } else {
        __info
          << "Unknown binding: " << binding_name;
        ++line;
      }
    } else {
      // Retry with next line
      ++line;
    }
  }
}

void enabler_input::save_keybindings(const ::std::string &file) {
  __info
    << "Saving bindings to " << file;
  ::std::string temporary = file + ".tmp";
  ::std::ofstream s(temporary.c_str());
  ::std::multimap< interface_key_t, event_match > map;
  interface_key_t last_key = INTERFACEKEY_NONE;

  if (!s.good()) {
    __error
      << "Failed to open " << temporary << " for writing";
    s.close();
    return;
  }
  // Invert keyboard map
  for (::std::multimap< event_match, interface_key_t >::iterator it = keymap.begin(); it != keymap.end(); ++it)
    map.insert(::std::pair< interface_key_t, event_match >(it->second, it->first));
  // Insert an empty line for the benefit of note/wordpad
  s << ::std::endl;
  // And write.
  for (::std::multimap< interface_key_t, event_match >::iterator it = map.begin(); it != map.end(); ++it) {
    if (!s.good()) {
      __error
        << "I/O error while writing keyboard mapping";
      s.close();
      return;
    }
    if (it->first != last_key) {
      last_key = it->first;
      s << "[BIND:" << bindingNames.left.at(it->first) << ":" << translate_repeat(repeatmap[it->first]) << "]" << ::std::endl;
    }
    switch (it->second.type) {
      case type_unicode:
        s << "[KEY:" << encode_utf8(it->second.unicode) << "]" << ::std::endl;
        break;
      case type_key:
        s << "[SYM:" << (int) it->second.mod << ":" << sdlNames.left.at(it->second.key) << "]" << ::std::endl;
        break;
      case type_button:
        s << "[BUTTON:" << (int) it->second.mod << ":" << (int) it->second.button << "]" << ::std::endl;
        break;
    }
  }
  s.close();
  replace_file(temporary, file);
}

void enabler_input::save_keybindings() {
  save_keybindings(interfacefile);
}

::std::string enabler_input::GetBindingTextDisplay(interface_key_t binding) {
  ::boost::bimap< interface_key_t, ::std::string >::left_iterator it = displayNames.left.find(binding);
  if (it != displayNames.left.end())
    return it->second;
  else
    return "NO BINDING";
}

void enabler_input::record_input() {
  active_macro.clear();
  macro_recording = true;
}

void enabler_input::record_stop() {
  macro_recording = false;
}

bool enabler_input::is_recording() {
  return macro_recording;
}

void enabler_input::play_macro() {
  ::boost::lock_guard< ::boost::mutex > timeline_lock(timeline_mutex);

  uint32_t now = SDL_GetTicks();
  for (interface_key_set_list_t::iterator sim = active_macro.begin(); sim != active_macro.end(); ++sim) {
    event< interface_event > e;
    e.r = REPEAT_NOT;
    e.repeats = 0;
    for (::std::set< interface_key_t >::iterator k = sim->begin(); k != sim->end(); ++k) {
      e.k = *k;

      event_engine::instance().fire(e);
      now += init.input.macro_time;
    }
  }
  macro_end = ::std::max(macro_end, now);
}

bool enabler_input::is_macro_playing() {
  return SDL_GetTicks() <= macro_end;
}

::std::list< ::std::string > enabler_input::list_macros() {
  // First, check for unloaded macros
  ::std::vector< ::std::string > files;
  find_files_by_glob("data/init/macros/*.mak", files);

  ::std::string file;
  for (::std::vector< ::std::string >::iterator it = files.begin(); it != files.end(); ++it) {
    file = "data/init/macros/";
    file.append(*it);
    load_macro_from_file(file);
  }

  // Then return all in-memory macros
  ::std::list< ::std::string > ret;
  for (::std::map< ::std::string, interface_key_set_list_t >::iterator it = macros.begin(); it != macros.end(); ++it)
    ret.push_back(it->first);
  return ret;
}

void enabler_input::load_macro(::std::string name) {
  if (macros.find(name) != macros.end())
    active_macro = macros[name];
  else
    macros.clear();
}

void enabler_input::save_macro(::std::string name) {
  macros[name] = active_macro;
  save_macro_to_file("data/init/macros/" + filter_filename(name, '_') + ".mak", name, active_macro);
}

void enabler_input::delete_macro(::std::string name) {
  ::std::map< ::std::string, interface_key_set_list_t >::iterator it = macros.find(name);
  if (it != macros.end())
    macros.erase(it);
  // TODO: Store the filename it was loaded from instead
  ::std::string filename = "data/init/macros/" + filter_filename(name, '_') + ".mak";
  remove(filename.c_str());
}

// Sets the next key-press to be stored instead of executed.
void enabler_input::register_key() {
  key_registering = true;
  stored_keys.clear();
}

// Returns a description of stored keys. Max one of each type.
::std::list< registered_key > enabler_input::getRegisteredKey() {
  key_registering = false;
  ::std::list< registered_key > ret;
  for (::std::list< event_match >::iterator it = stored_keys.begin(); it != stored_keys.end(); ++it) {
    struct registered_key r = { it->type, display(*it) };
    ret.push_back(r);
  }
  return ret;
}

// Binds one of the stored keys to key
void enabler_input::bindRegisteredKey(match_type type, interface_key_t key) {
  for (::std::list< event_match >::iterator it = stored_keys.begin(); it != stored_keys.end(); ++it) {
    if (it->type == type) {
      keymap.insert(::std::pair< event_match, interface_key_t >(*it, key));
      update_keydisplay(key, display(*it));
    }
  }
}

bool enabler_input::is_registering() {
  return key_registering;
}

::std::list< event_match > enabler_input::list_keys(interface_key_t key) {
  ::std::list< event_match > ret;

  // Oh, now this is inefficient.
  for (keymap_t::iterator it = keymap.begin(); it != keymap.end(); ++it) {
    if (it->second == key) {
      ret.push_back(it->first);
    }
  }

  return ret;
}

void enabler_input::remove_key(interface_key_t key, event_match ev) {
  for (keymap_t::iterator it = keymap.find(ev); it != keymap.end() && it->first == ev; ++it) {
    if (it->second == key)
      keymap.erase(it++);
  }
  // Also remove the key from key displaying, assuming we can find it
  keydisplay_t::iterator it = enabler_input::keydisplay.find(key);
  if (it != enabler_input::keydisplay.end()) {
    it->second.erase(display(ev));
  }
}
