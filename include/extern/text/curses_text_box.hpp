/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_TEXT_SYSTEM_CURSES_TEXT_BOX_HPP_
#define EXTERN_TEXT_SYSTEM_CURSES_TEXT_BOX_HPP_

#include <string>
#include "integer_types.hpp"

#include "extern/string_vect.hpp"

class curses_text_boxst {
  public:
    stringvectst text;

    void add_paragraph(stringvectst &src, int32_t para_width);
    /**
     * @ingroup extern
     */
    void add_paragraph(const ::std::string &src, int32_t para_width);

    void read_file(file_compressorst &filecomp, int32_t loadversion);
    void write_file(file_compressorst &filecomp);
    void clean();
};

#endif /* EXTERN_TEXT_SYSTEM_CURSES_TEXT_BOX_HPP_ */
