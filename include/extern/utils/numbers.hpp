/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_NUMBERS_HPP_
#define EXTERN_UTILS_NUMBERS_HPP_

#include <string>
#include "integer_types.hpp"

/**
 * @{
 * @ingroup extern
 */
void get_number(int32_t, ::std::string&);
void get_ordinal(int32_t, ::std::string&, bool);

void add_long_to_string(int32_t, ::std::string&);

int32_t convert_string_to_long(::std::string&);
uint32_t convert_string_to_ulong(::std::string&);
void convert_long_to_string(int32_t, ::std::string&);
void convert_ulong_to_string(uint32_t, ::std::string&);
/** @} */

#endif /* EXTERN_UTILS_NUMBERS_HPP_ */
