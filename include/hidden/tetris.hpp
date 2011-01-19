/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef HIDDEN_TETRIS_HPP_
#define HIDDEN_TETRIS_HPP_

#include <list>
#include <set>
#include <string>

#include "viewscreen.hpp"

struct line;

class matrix_screen: public viewscreenst {
    typedef ::std::list< line >::iterator iterator_t;

    ::std::list< line > lines;
    int frame;

    ::std::string random_string(int len);
  public:
    matrix_screen();
    virtual void feed(::std::set< interface_key_t > &events);
    virtual void render();
    virtual void logic();
    virtual void resize(int w, int h);
};

#endif /* HIDDEN_TETRIS_HPP_ */
