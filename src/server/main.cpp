/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

#include "server.hpp"
#include "g_src/enabler.h"
#include "g_src/init.h"

#include "signals.hpp"
#include "hilbert.hpp"
#include "varint.hpp"

int simulation_thread(void*) {
  enabler.async_loop();
  return 0;
}

int main(int argc, char **argv) {
  df::add_kill_handler();

  df::server::instance_ = std::auto_ptr< df::server >(new df::server("tcp://0.0.0.0:6000",
                                                                     "tcp://0.0.0.0:6001",
                                                                     "tcp://0.0.0.0:6002",
                                                                     "<server-uuid>",
                                                                     "<password-hash>"));
  boost::thread control_thread = boost::thread(df::server::control_thread(*df::server::instance_.get()));

// Initialise minimal SDL subsystems.
  if (SDL_Init(SDL_INIT_TIMER)) {
    std::cerr << "SDL initialization failure: " << SDL_GetError() << "\n";
    return false;
  }
  enabler.renderer_threadid = SDL_ThreadID();

  // Spawn simulation thread
  SDL_CreateThread(simulation_thread, NULL);

  init.begin(); // Load init.txt settings

  // Load keyboard map
  keybinding_init();
  enabler.load_keybindings("data/init/interface.txt");

  std::string cmdLine;
  for (int i = 1; i < argc; ++i) {
    char *option = argv[i];
    cmdLine += option;
    cmdLine += " ";
  }
  int result = enabler.loop(cmdLine);

  return result;
}
