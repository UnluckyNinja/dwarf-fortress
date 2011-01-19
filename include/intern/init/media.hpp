/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_INIT_MEDIA_HPP_
#define INTERN_INIT_MEDIA_HPP_

#include "integer_types.hpp"

#include "extern/flag_array.hpp"

enum InitMediaFlag {
  INIT_MEDIA_FLAG_SOUND_OFF, INIT_MEDIA_FLAG_INTRO_OFF, INIT_MEDIA_FLAG_COMPRESS_SAVES, INIT_MEDIA_FLAGNUM
};

class init_mediast {
  public:
    flagarrayst flag;
    int32_t volume;

    init_mediast();
};

#endif /* INTERN_INIT_MEDIA_HPP_ */
