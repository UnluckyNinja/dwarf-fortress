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

#include "intern/config.hpp"

int call_loop(void *dummy);

int main(int argc, char* argv[]) {
  config& conf = config::instance();

  ::boost::thread event_thread = event_engine::start();

  conf.init(argc, argv);

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

  bool const display_mode_is_text = conf.display().mode == display_config::display_mode::text;
  bool const use_graphics = conf.texture().use_graphics;

#if !defined(__APPLE__) && defined(unix)
  if (!gtk_ok && !display_mode_is_text) {
    __fatal
      << "Display not found and PRINT_MODE not set to TEXT, aborting.";
    exit(EXIT_FAILURE);
  }
  if (display_mode_is_text && use_graphics) {
    __fatal
      << "Graphical tiles are not compatible with text output, sorry";
    exit(EXIT_FAILURE);
  }
#endif

  // Initialize video, if we /use/ video
  retval = SDL_InitSubSystem(display_mode_is_text ? 0 : SDL_INIT_VIDEO);
  if (retval != 0) {
    __error
      << "SDL initialization failure" << SDL_GetError();
    return false;
  }

#ifdef linux
  if (!conf.gameplay().disable_music) {
    // Initialize OpenAL
    if (!musicsound.initsound()) {
      __info
        << "Initializing OpenAL failed, no sound will be played";
      !conf.gameplay().disable_music = true;
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
