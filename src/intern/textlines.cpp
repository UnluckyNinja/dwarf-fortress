/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/textlines.hpp"

void textlinesst::clean_lines() {
  text.clean();
}
void textlinesst::copy_from(textlinesst &ot) {
  text.copy_from(ot.text);
}
