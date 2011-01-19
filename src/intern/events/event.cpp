/**
 * @file
 * @date 16 juil. 2010
 * @todo comment
 */

#include "intern/events/event.hpp"

const ::boost::posix_time::time_duration event_base::min_delay = ::boost::posix_time::milliseconds(0);

event_base::event_base(const ::boost::posix_time::time_duration& delay) {
  datetime = ::boost::get_system_time();
  datetime += ::std::max(delay, min_delay);
  cancelled = false;
}

event_base::event_base(const event_base& event) {
  datetime = event.get_datetime();
  cancelled = event.is_cancelled();
}

void event_base::cancel() {
  cancelled = true;
}

bool event_base::is_cancelled() const {
  return cancelled;
}

::boost::system_time event_base::get_datetime() const {
  return datetime;
}

bool event_base::operator<(const event_base& event) const {
  if (cancelled == event.is_cancelled()) {
    return datetime > event.get_datetime();
  } else {
    return cancelled ? false : true;
  }
}
