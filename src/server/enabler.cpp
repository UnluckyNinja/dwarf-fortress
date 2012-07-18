/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "g_src/enabler.h"

#include "signals.hpp"

#include "server.hpp"
#include "server_renderer.hpp"
#include "message/input.hpp"

extern std::set< InterfaceKey > inputs;

/* Simulation thread.
 *
 * This thread is doing two things. First it looks for events in the event queue.
 * Events can be pause/unpause simulation, render, increment or set_fps.
 *
 * Render event is updating graphics.
 * Increment event is providing the number of simulation frames to render, to respect the desired FPS.
 * Set FPS event is setting the current simulation FPS, as a check that the immediate FPS will not be > 3 * FPS.
 *
 * If the ENABLERFLAG_MAXFPS flag is not set, async_frames is used to check the maximum number of simulation frames
 * to compute.
 */
void enablerst::async_loop() {
  async_paused = false;
  async_frames = 0;
  int fps = 100; // Just a thread-local copy

  while (true) {
    // Check for commands
    async_cmd cmd;
    bool have_cmd = true;
    do {
      if (async_paused || (async_frames == 0 && !(enabler.flag & ENABLERFLAG_MAXFPS)))
        async_tobox.read(cmd);
      else
        have_cmd = async_tobox.try_read(cmd);

      // Obey the command, would you kindly.
      if (have_cmd) {
        switch (cmd.cmd) {
          case async_cmd::pause:
            async_paused = true;
            async_frombox.write(async_msg(async_msg::complete));
            break;

          case async_cmd::start:
            async_paused = false;
            async_frames = 0;
            break;

          case async_cmd::render:
            if (flag & ENABLERFLAG_RENDER) {
              renderer->swap_arrays();
              render_things();
              flag &= ~ENABLERFLAG_RENDER;
              update_gfps();
            }
            async_frombox.write(async_msg(async_msg::complete));
            break;

          case async_cmd::inc:
            async_frames += cmd.val;
            if (async_frames > fps * 3)
              async_frames = fps * 3; // Just in case
            break;

          case async_cmd::set_fps:
            fps = cmd.val;
            break;
        }
      }
    } while (have_cmd);

    // Run the main-loop, maybe
    if (!async_paused && (async_frames || (enabler.flag & ENABLERFLAG_MAXFPS))) {

      if (mainloop()) {
        async_frombox.write(async_msg(async_msg::quit));
        return; // We're done.
      }

      simticks.lock();
      simticks.val++;
      simticks.unlock();

      async_frames--;
      if (async_frames < 0)
        async_frames = 0;

      update_fps();
    }

    SDL_NumJoysticks(); // Hook for dfhack
  }
}

int enablerst::loop(std::string cmdline) {
  command_line = cmdline;

  // Initialize the tick counters
  simticks.write(0);
  gputicks.write(0);

  // Call DF's initialization routine
  if (!beginroutine())
    exit(EXIT_FAILURE);

  // Allocate a renderer
  renderer = new df::server_renderer();

  // At this point we should have a window that is setup to render DF.
  SDL_EnableUNICODE(1);
  eventLoop_SDL();

  endroutine();

  // Clean up graphical resources
  //  delete renderer;
}

void enablerst::eventLoop_SDL() {
  // Initialize the grid
  renderer->resize(80, 25);

  while (!df::kill_received()) {
    uint32_t now = SDL_GetTicks();
    bool paused_loop = false;
    if (!paused_loop) {
      pause_async_loop();
      paused_loop = true;
    }

    df::message::input_t message;
    while (df::server::instance().receive(message)) {
      inputs.insert(message.keys.begin(), message.keys.end());
    }

    if (paused_loop)
      unpause_async_loop();

    do_frame();
  }
}
