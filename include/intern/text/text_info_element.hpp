/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXT_INFO_ELEMENT_HPP_
#define INTERN_TEXT_INFO_ELEMENT_HPP_

#include <string>

class text_info_elementst {
  public:
    virtual ::std::string get_string();
    virtual long get_long();

    virtual ~text_info_elementst();
};

#endif /* INTERN_TEXT_INFO_ELEMENT_HPP_ */
