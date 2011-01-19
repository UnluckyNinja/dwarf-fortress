/**
 * @file
 * @date 25 juin 2010
 * @todo comment
 */

#include "intern/enabler/input/mod_state.hpp"

#include <iostream>

mod_state::mod_state() :
  value(0) {
}
;

mod_state::mod_state(const value_t value) :
  value(value) {
}
;

void mod_state::clear() {
  value = 0;
}

bool mod_state::update(const sdl_keyboard_event_t& e) {
  uint16_t modifier;
  switch (e.keysym.sym) {
    case SDLK_RSHIFT:
    case SDLK_LSHIFT:
      modifier = MOD_SHIFT;
      break;
    case SDLK_RCTRL:
    case SDLK_LCTRL:
      modifier = MOD_CTRL;
      break;
    case SDLK_RALT:
    case SDLK_LALT:
      modifier = MOD_ALT;
      break;
    case SDLK_LMETA:
    case SDLK_RMETA:
      modifier = MOD_META;
      break;
    case SDLK_LSUPER:
    case SDLK_RSUPER:
      modifier = MOD_SUPER;
      break;
    case SDLK_MODE:
      modifier = MOD_MODE;
      break;
    case SDLK_COMPOSE:
      modifier = MOD_COMPOSE;
      break;
    default:
      modifier = 0;
      break;
  }

  uint16_t new_value = value;
  switch (e.type) {
    case SDL_KEYUP:
      new_value &= ~modifier;
      break;
    case SDL_KEYDOWN:
      new_value |= modifier;
      break;
  }

  return update(mod_state(new_value));
}

bool mod_state::update(const mod_state& mod_state_in) {
  if (value != mod_state_in.value) {
    value = mod_state_in.value;
    return true;
  }

  return false;
}

mod_state::operator uint32_t() const {
  return value;
}

::std::ostream& operator<<(::std::ostream& out, const mod_state& state) {
  if (state.value & mod_state::MOD_COMPOSE) {
    out << "Compose+";
  }
  if (state.value & mod_state::MOD_ALT) {
    out << "Alt+";
  }
  if (state.value & mod_state::MOD_CTRL) {
    out << "Ctrl+";
  }
  if (state.value & mod_state::MOD_META) {
    out << "Meta+";
  }
  if (state.value & mod_state::MOD_SHIFT) {
    out << "Shift+";
  }
  if (state.value & mod_state::MOD_SUPER) {
    out << "Super+";
  }
  if (state.value & mod_state::MOD_MODE) {
    out << "Mode+";
  }

  return out;
}
