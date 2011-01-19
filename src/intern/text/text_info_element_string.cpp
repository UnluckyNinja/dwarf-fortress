/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/text/text_info_element_string.hpp"

::std::string text_info_element_stringst::get_string() {
  return str;
}

text_info_element_stringst::text_info_element_stringst(const ::std::string &newstr) {
  str = newstr;
}
