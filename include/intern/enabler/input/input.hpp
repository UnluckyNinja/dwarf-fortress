/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_INPUT_HPP_
#define INTERN_ENABLER_INPUT_INPUT_HPP_

#include <set>
#include <list>

#include <string>
#include "integer_types.hpp"
#include "event_types.hpp"

int32_t decode_utf8(const ::std::string &s);
int32_t decode_utf8_predict_length(int8_t byte);
::std::string encode_utf8(int32_t unicode);

typedef ::std::set< interface_key_t > interface_key_set_t;
typedef ::std::list< interface_key_set_t > interface_key_set_list_t;

struct less_sz {
    bool operator()(const ::std::string &a, const ::std::string &b) const;
};

#endif /* INTERN_ENABLER_INPUT_INPUT_HPP_ */
