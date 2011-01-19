/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_UTILS_OPENAL_HPP_
#define INTERN_UTILS_OPENAL_HPP_

#include <AL/al.h>
#include <AL/alc.h>

// HACKY HACKY HACK
// Fixes sndfile.h, until the bug is properly fixed
#define _MSCVER
#include "integer_types.hpp"
typedef int64_t __int64;
#include <sndfile.h>
#undef _MSCVER
// END HACKY HACKY HACK

#include <logging/logging.hpp>

#define __al_check_errors \
  while (ALenum __al_error = alGetError()) { \
    __errorM(openal) << "#0x" << ::std::hex << __al_error << ::std::dec; \
  } \

#endif /* INTERN_UTILS_OPENAL_HPP_ */
