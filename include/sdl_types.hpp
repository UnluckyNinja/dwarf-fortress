/**
 * @file
 * @date 24 juin 2010
 * @todo comment
 */

#ifndef SDL_TYPES_HPP_
#define SDL_TYPES_HPP_

struct sdl_no_type;

#include <SDL_keysym.h>
#include <SDL_events.h>

typedef SDLKey sdl_key_t;
typedef SDL_Event sdl_event_t;
typedef SDL_KeyboardEvent sdl_keyboard_event_t;

#define SDLKey sdl_no_type;
#define SDL_Event sdl_no_type;
#define SDL_KeyboardEvent sdl_no_type;

#endif /* SDL_TYPES_HPP_ */
