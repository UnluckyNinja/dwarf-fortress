/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#ifndef INTERN_UTILS_NUMBERS_HPP_
#define INTERN_UTILS_NUMBERS_HPP_

#include "integer_types.hpp"
#include <string>

#include <logging/logging.hpp>

int32_t to_int32(::std::string);
uint32_t to_uint32(::std::string);

namespace numbers {
  template< typename integer_type_t >
  inline integer_type_t clamp(const integer_type_t& value, const integer_type_t& min, const integer_type_t& max) {
    return ::std::min(::std::max(value, min), max);
  }
}

#endif /* INTERN_UTILS_NUMBERS_HPP_ */
