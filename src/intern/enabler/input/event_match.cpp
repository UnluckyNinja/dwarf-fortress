/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include "intern/enabler/input/event_match.hpp"

bool event_match::operator ==(const event_match& other) const {
  if ((int32_t) mod != (int32_t) other.mod) {
    return false;
  }

  if (type != other.type) {
    return false;
  }

  switch (type) {
    case type_unicode:
      return unicode == other.unicode;
    case type_key:
      return key == other.key;
    case type_button:
      return button == other.button;
    default:
      return false;
  }
}

bool event_match::operator <(const event_match& other) const {
  if ((int32_t) mod != (int32_t) other.mod) {
    return (int32_t) mod < (int32_t) other.mod;
  }

  if (type != other.type) {
    return type < other.type;
  }

  switch (type) {
    case type_unicode:
      return unicode < other.unicode;
    case type_key:
      return key < other.key;
    case type_button:
      return button < other.button;
    default:
      return false;
  }
}
