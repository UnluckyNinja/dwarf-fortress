/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_KEY_EVENT_HPP_
#define INTERN_ENABLER_INPUT_KEY_EVENT_HPP_

#include <string>

#include "event_match.hpp"

struct key_event {
    static const ::std::string event_name;

    bool release;
    event_match match;
};

#endif /* INTERN_ENABLER_INPUT_KEY_EVENT_HPP_ */
