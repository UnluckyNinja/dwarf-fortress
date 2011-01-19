/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/renderer/2d.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"

#include <logging/logging.hpp>

#include <SDL.h>
#include <SDL_image.h>

renderer_2d::renderer_2d() {
  // Disable key repeat
  SDL_EnableKeyRepeat(0, 0);
  // Set window title/icon.
  SDL_WM_SetCaption(GAME_TITLE_STRING, NULL);
  SDL_Surface *icon = IMG_Load("data/art/icon.png");
  if (icon != NULL) {
    SDL_WM_SetIcon(icon, NULL);
    // The icon's surface doesn't get used past this point.
    SDL_FreeSurface(icon);
  }

  // Find the current desktop resolution if fullscreen resolution is auto
  if (init.display.desired_fullscreen_width == 0 || init.display.desired_fullscreen_height == 0) {
    const struct SDL_VideoInfo *info = SDL_GetVideoInfo();
    init.display.desired_fullscreen_width = info->current_w;
    init.display.desired_fullscreen_height = info->current_h;
  }

  // Initialize our window
  bool worked = init_video(enabler.is_fullscreen() ? init.display.desired_fullscreen_width
      : init.display.desired_windowed_width, enabler.is_fullscreen() ? init.display.desired_fullscreen_height
      : init.display.desired_windowed_height);

  // Fallback to windowed mode if fullscreen fails
  if (!worked && enabler.is_fullscreen()) {
    enabler.fullscreen = false;
    __errorM(sdl)
      << "SDL initialization failure, trying windowed mode" << SDL_GetError();
    worked = init_video(init.display.desired_windowed_width, init.display.desired_windowed_height);
  }
  // Quit if windowed fails
  if (!worked) {
    __errorM(sdl)
      << "SDL initialization failure" << SDL_GetError();
    exit(EXIT_FAILURE);
  }
}
