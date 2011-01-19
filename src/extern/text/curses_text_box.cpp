/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/text/curses_text_box.hpp"

void curses_text_boxst::add_paragraph(const ::std::string &src, int32_t para_width) {
  stringvectst sp;
  sp.add_string(src);
  add_paragraph(sp, para_width);
}
