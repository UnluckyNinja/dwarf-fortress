/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TILE_PAGE_HPP_
#define INTERN_TILE_PAGE_HPP_

#include <string>
#include "integer_types.hpp"

#include "extern/svector.hpp"

struct tile_pagest {
    ::std::string token;

    ::std::string filename;
    short tile_dim_x;
    short tile_dim_y;
    short page_dim_x;
    short page_dim_y;

    svector< int32_t > texpos;
    svector< int32_t > datapos;
    svector< int32_t > texpos_gs;
    svector< int32_t > datapos_gs;

    char loaded;

    tile_pagest() {
      loaded = 0;
    }

    void load_graphics(::std::string& graphics_dir);
};

#endif /* INTERN_TILE_PAGE_HPP_ */
