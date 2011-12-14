/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include <locale>
#include <iostream>

#include <gtk/gtk.h>

#include "g_src/enabler.h"
#include "g_src/init.h"
#include "g_src/music_and_sound_g.h"

void report_error(const char *error_preface, const char *error_message);

int call_loop(void *dummy) {
  enabler.async_loop();
  return 0;
}

int main (int argc, char* argv[]) {
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
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }
  enabler.renderer_threadid = SDL_ThreadID();

  // Spawn simulation thread
  SDL_CreateThread(call_loop, NULL);

  init.begin(); // Load init.txt settings

#if !defined(__APPLE__) && defined(unix)
  if (!gtk_ok && !init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
    puts("Display not found and PRINT_MODE not set to TEXT, aborting.");
    exit(EXIT_FAILURE);
  }
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) &&
      init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS)) {
    puts("Graphical tiles are not compatible with text output, sorry");
    exit(EXIT_FAILURE);
  }
#endif

  // Initialize video, if we /use/ video
  retval = SDL_InitSubSystem(init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT) ? 0 : SDL_INIT_VIDEO);
  if (retval != 0) {
    report_error("SDL initialization failure", SDL_GetError());
    return false;
  }

#ifdef linux
  if (!init.media.flag.has_flag(INIT_MEDIA_FLAG_SOUND_OFF)) {
    // Initialize OpenAL
    if (!musicsound.initsound()) {
      puts("Initializing OpenAL failed, no sound will be played");
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
  enabler.load_keybindings("data/init/interface.txt");

  string cmdLine;
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

  return result;
}

