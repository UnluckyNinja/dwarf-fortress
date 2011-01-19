/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/text/text_info.hpp"

#include "intern/text/text_info_element.hpp"

void text_infost::clean() {
  while (element.size() > 0) {
    delete element[0];
    element.erase(0);
  }
}

::std::string text_infost::get_string(int e) {
  if (e < 0 || e >= static_cast< int > (element.size())) {
    ::std::string empty;
    return empty;
  }
  if (element[e] == NULL) {
    ::std::string empty;
    return empty;
  }
  return element[e]->get_string();
}

long text_infost::get_long(int e) {
  if (e < 0 || e >= static_cast< int > (element.size())) {
    return 0;
  }
  if (element[e] == NULL) {
    return 0;
  }
  return element[e]->get_long();
}

text_infost::~text_infost() {
  clean();
}
