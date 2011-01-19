/**
 * @file
 * @date 26 juin 2010
 * @todo comment
 */

#include "extern/texture_handler.hpp"

#include "intern/tile_page.hpp"

texture_handlerst::~texture_handlerst() {
  clean();
}

tile_pagest* texture_handlerst::get_tile_page_by_token(::std::string &tk) {
  for (size_t t = 0; t < page.size(); t++) {
    if (page[t]->token == tk) {
      return page[t];
    }
  }

  return 0;
}
