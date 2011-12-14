/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_RENDERER_CURSES_HPP_
#define DWARF_FORTRESS_RENDERER_CURSES_HPP_

#include "g_src/enabler.h"

#include <map>
#include <list>

class renderer_curses: public renderer {
    // For curses. MOVEME.
    std::map< std::pair< int, int >, int > color_pairs;

    // Map from DF color to ncurses color
    static int ncurses_map_color(int color);

    // Look up, or create, a curses color pair
    int lookup_pair(std::pair< int, int > color);

  public:
    void update_tile(int x, int y);
    void update_all();
    void render();
    void resize(int w, int h);
    void grid_resize(int w, int h);
    renderer_curses();
    bool get_mouse_coords(int &x, int &y);

    virtual ~renderer_curses() {
    }
};

#endif /* DWARF_FORTRESS_RENDERER_CURSES_HPP_ */
