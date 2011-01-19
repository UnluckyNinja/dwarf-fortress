/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXT_INFO_ELEMENT_STRING_HPP_
#define INTERN_TEXT_INFO_ELEMENT_STRING_HPP_

#include <string>

#include "intern/text/text_info_element.hpp"

class text_info_element_stringst: public text_info_elementst {
  public:
    virtual ::std::string get_string();
    text_info_element_stringst(const ::std::string &newstr);

  protected:
    ::std::string str;
};

#endif /* INTERN_TEXT_INFO_ELEMENT_STRING_HPP_ */
