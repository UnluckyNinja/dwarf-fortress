/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/text/text_info_element_long.hpp"

long text_info_element_longst::get_long() {
  return val;
}

text_info_element_longst::text_info_element_longst(long nval) {
  val = nval;
}

