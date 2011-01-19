/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_RENDERER_OFFSCREEN_HPP_
#define EXTERN_RENDERER_OFFSCREEN_HPP_

#include <string>

#include "extern/renderer/base.hpp"

class renderer_offscreen: public renderer_2d_base {
    virtual bool init_video(int, int);
  public:
    /**
     * @ingroup extern
     */
    ~renderer_offscreen();
    /**
     * @ingroup extern
     */
    renderer_offscreen(int, int);
    /**
     * @ingroup extern
     */
    void update_all(int, int);
    /**
     * @ingroup extern
     */
    void save_to_file(const ::std::string &file);
};

#endif /* EXTERN_RENDERER_OFFSCREEN_HPP_ */
