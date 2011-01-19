/**
 * @file
 * @date 18 janv. 2011
 * @todo comment
 */

#ifndef INTERN_TTF_MANAGER_HPP_
#define INTERN_TTF_MANAGER_HPP_

#include <list>
#include <unordered_map>
#include <SDL/SDL_ttf.h>

#include "extern/textures.hpp"

class ttf_managerst {
    TTF_Font *font;
    int max_handle;
    int tile_width, ceiling;
    ::std::unordered_map< ttf_id, ::std::pair< int, int > > handles; // First is handle, second is width
    ::std::unordered_map< int, SDL_Surface* > textures;
    ::std::list< ::std::pair< int, ttf_id > > todo;

    int size_ttf(const ::std::string &text, int);
  public:
    ttf_managerst() {
      font = NULL;
      max_handle = 1;
    }
    bool init(int ceiling, int tile_width);
    bool was_init() {
      return font != NULL;
    }
    // Returns a handle for some TTF text (first), plus its onscreen width (second)
    ::std::pair< int, int > get_handle(const ttf_id &id);
    // Returns rendered text. Renders too, if necessary.
    // The returned SDL_Surface is owned by the ttf_managerst.
    SDL_Surface *get_texture(int handle);
};

extern ttf_managerst ttf_manager;

#endif /* INTERN_TTF_MANAGER_HPP_ */
