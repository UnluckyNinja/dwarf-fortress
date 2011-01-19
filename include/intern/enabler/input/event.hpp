/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_EVENT_HPP_
#define INTERN_ENABLER_INPUT_EVENT_HPP_

#include <boost/thread.hpp>

#include <string>
#include "integer_types.hpp"
#include "event_types.hpp"

enum repeat_speed {
  // Don't repeat at all. Furthermore, cancel other repeats.
  REPEAT_NOT,
  // Repeat normally.
  REPEAT_SLOW,
  // Repeat instantly, without waiting for the first-repeat interval.
  REPEAT_FAST
};

// The timeline events we actually pass back from get_input. Well, no,
// that's just k, but..
struct interface_event {
    static const ::std::string event_name;

    repeat_speed r;
    interface_key_t k;
    int32_t repeats; // Starts at 0, increments once per repeat

    bool operator==(const interface_event &other) const;

    // We sort by time first, and then serial number.
    // The order of the other bits is unimportant.
    bool operator<(const interface_event &o) const;
};

#endif /* INTERN_ENABLER_INPUT_EVENT_HPP_ */
