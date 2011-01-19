/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#include "intern/utils/numbers.hpp"

#include <logging/logging.hpp>

#include <boost/lexical_cast.hpp>

template< typename integer_type_t >
integer_type_t to_integer(::std::string str) {
  try {
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);

    return ::boost::lexical_cast< integer_type_t >(str);
  } catch (::boost::bad_lexical_cast& e) {
    __error
      << e.what() << " while converting: '" << str << "', returning " << atoi(str.c_str());
    return atoi(str.c_str());
  }
}

int32_t to_int32(::std::string str) {
  return to_integer< int32_t > (str);
}

uint32_t to_uint32(::std::string str) {
  return to_integer< uint32_t > (str);
}
