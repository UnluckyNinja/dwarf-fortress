/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_EVENT_MATCH_HPP_
#define INTERN_ENABLER_INPUT_EVENT_MATCH_HPP_

#include "integer_types.hpp"
#include "sdl_types.hpp"

#include "mod_state.hpp"

enum match_type {
  type_unicode, type_key, type_button
};

struct event_match {
    match_type type;

    // not defined for type=unicode. 1: shift, 2: ctrl, 4:alt
    mod_state mod;

    // not defined for type=button
    uint8_t scancode;
    union {
        uint16_t unicode;
        uint8_t button;
        sdl_key_t key;
    };

    bool operator==(const event_match &other) const;
    bool operator<(const event_match &other) const;
};

#endif /* INTERN_ENABLER_INPUT_EVENT_MATCH_HPP_ */
