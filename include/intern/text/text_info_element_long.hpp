/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXT_INFO_ELEMENT_LONG_HPP_
#define INTERN_TEXT_INFO_ELEMENT_LONG_HPP_

#include "intern/text/text_info_element.hpp"

class text_info_element_longst: public text_info_elementst {
  public:
    virtual long get_long();
    text_info_element_longst(long nval);

  protected:
    long val;
};

#endif /* INTERN_TEXT_INFO_ELEMENT_LONG_HPP_ */
