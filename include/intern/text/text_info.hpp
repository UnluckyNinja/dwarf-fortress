/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXT_INFO_HPP_
#define INTERN_TEXT_INFO_HPP_

struct text_info_elementst;

#include "extern/string_vect.hpp"

class text_infost {
  public:
    svector< text_info_elementst * > element;

    void clean();
    ::std::string get_string(int e) ;
    long get_long(int e);
    ~text_infost() ;
};

#endif /* INTERN_TEXT_INFO_HPP_ */
