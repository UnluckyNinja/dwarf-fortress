/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_TEXTURE_HANDLER_HPP_
#define EXTERN_TEXTURE_HANDLER_HPP_

#include <string>
#include "integer_types.hpp"

#include "extern/svector.hpp"

struct tile_pagest;
struct textlinesst;

class texture_handlerst {
  private:
    svector< tile_pagest* > page;

    svector< int32_t > texpos;
    svector< int32_t > datapos;

  public:

    /**
     * @ingroup hidden
     */
    void clean();
    void adopt_new_lines(textlinesst& lines, ::std::string& graphics_dir);

    ~texture_handlerst();

    tile_pagest* get_tile_page_by_token(::std::string &tk);
};

#endif /* EXTERN_TEXTURE_HANDLER_HPP_ */
