/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */
#define BOOST_PARAMETER_MAX_ARITY 7

#include <gtk/gtk.h>
#include <SDL.h>

#include "extern/main.hpp"

#include "hidden/global.hpp"
#include "hidden/dwarf.hpp"

#include "extern/init.hpp"
#include "extern/music.hpp"
#include "extern/musicsound.hpp"
#include "extern/enabler.hpp"
#include "intern/enabler/input.hpp"

#include "intern/events/event_engine.hpp"

#include <logging/logging.hpp>

#include "intern/keybindings.hpp"

int call_loop(void *dummy);

int main(int argc, char* argv[]) {
  ::boost::thread event_thread = event_engine::start();

#ifdef unix
  setlocale(LC_ALL, "");
#endif
#if !defined(__APPLE__) && defined(unix)
  bool gtk_ok = false;
  if (getenv("DISPLAY"))
    gtk_ok = gtk_init_check(&argc, &argv);
#endif

  // Initialise minimal SDL subsystems.
  int retval = SDL_Init(SDL_INIT_TIMER);
  // Report failure?
  if (retval != 0) {
    __error
      << "SDL initialization failure" << SDL_GetError();
    return false;
  }
  enabler.renderer_threadid = SDL_ThreadID();

  // Spawn simulation thread
  SDL_CreateThread(call_loop, NULL);

  init.begin(); // Load init.txt settings

#if !defined(__APPLE__) && defined(unix)
  if (!gtk_ok && !init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    __info
      << "Display not found and PRINT_MODE not set to TEXT, aborting.";
    exit(EXIT_FAILURE);
  }
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) && init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS)) {
    __info
      << "Graphical tiles are not compatible with text output, sorry";
    exit(EXIT_FAILURE);
  }
#endif

  // Initialize video, if we /use/ video
  retval = SDL_InitSubSystem(init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) ? 0 : SDL_INIT_VIDEO);
  if (retval != 0) {
    __error
      << "SDL initialization failure" << SDL_GetError();
    return false;
  }

#ifdef linux
  if (!init.media.flag.has_flag(INIT_MEDIA_FLAG_SOUND_OFF)) {
    // Initialize OpenAL
    if (!musicsound.initsound()) {
      __info
        << "Initializing OpenAL failed, no sound will be played";
      init.media.flag.add_flag(INIT_MEDIA_FLAG_SOUND_OFF);
    }
  }
#endif

#ifdef WIN32
  // Attempt to get as good a timer as possible
  int ms = 1;
  while (timeBeginPeriod(ms) != TIMERR_NOERROR) ms++;
#endif

  // Load keyboard map
  keybinding_init();
  enabler_input::instance().load_keybindings("data/init/interface.txt");

  ::std::string cmdLine;
  for (int i = 1; i < argc; ++i) {
    char *option = argv[i];
    cmdLine += option;
    cmdLine += " ";
  }
  int result = enabler.loop(cmdLine);

  SDL_Quit();

#ifdef WIN32
  timeEndPeriod(ms);
#endif

  __info
    << "Exiting...";

  event_engine::instance().stop();
  event_thread.join();

  return result;
}
