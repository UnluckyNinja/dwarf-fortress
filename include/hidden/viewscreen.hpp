/**
 * @file
 * @date 26 juin 2010
 * @todo comment
 */

#ifndef HIDDEN_VIEWSCREEN_HPP_
#define HIDDEN_VIEWSCREEN_HPP_

#include <set>

#include "event_types.hpp"

/**
 * @ingroup hidden
 */
class viewscreenst {
  public:
    viewscreenst* child;
    viewscreenst* parent;
    char breakdownlevel;
    char option_key_pressed;

    virtual void feed(::std::set< interface_key_t >& events);
    virtual void logic();
    virtual void render();

    /**
     * @ingroup hidden
     */
    virtual void resize(int w, int h);

    /**
     * @ingroup hidden
     */
    virtual void help();

    /**
     * @ingroup hidden
     */
    virtual char movies_okay();

    virtual char is_option_screen();

    /**
     * @ingroup hidden
     */
    virtual char is_save_screen();

    viewscreenst();
    virtual ~viewscreenst();

    /**
     * @ingroup hidden
     */
    virtual bool key_conflict(interface_key_t test_key);
};

#endif /* HIDDEN_VIEWSCREEN_HPP_ */
