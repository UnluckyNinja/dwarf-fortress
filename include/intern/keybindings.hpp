/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_KEYBINDINGS_HPP_
#define INTERN_KEYBINDINGS_HPP_

#include <map>
#include <string>

#include <boost/bimap.hpp>

#include "sdl_types.hpp"
#include "event_types.hpp"

typedef ::boost::bimap< interface_key_t, ::std::string > binding_names_t;
typedef ::boost::bimap< interface_key_t, ::std::string > display_names_t;
typedef ::boost::bimap< sdl_key_t, ::std::string > sdl_names_t;

extern binding_names_t bindingNames;
extern display_names_t displayNames;
extern sdl_names_t sdlNames;

void keybinding_init();

#endif /* INTERN_KEYBINDINGS_HPP_ */
