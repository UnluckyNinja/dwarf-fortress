/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include "intern/enabler/input/event.hpp"

const ::std::string interface_event::event_name("interface_event");

bool interface_event::operator==(const interface_event &other) const {
  if (r != other.r) {
    return false;
  }

  if (k != other.k) {
    return false;
  }

  if (repeats != other.repeats) {
    return false;
  }

  return true;
}

// We sort by time first, and then serial number.
// The order of the other bits is unimportant.
bool interface_event::operator<(const interface_event &o) const {
  if (r != o.r) {
    return r < o.r;
  }

  if (k != o.k) {
    return k < o.k;
  }

  if (repeats != o.repeats) {
    return repeats < o.repeats;
  }

  return false;
}
