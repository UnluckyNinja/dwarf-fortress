/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "g_src/enabler.h"
#include "g_src/graphics.h"
#include "g_src/init.h"
#include "g_src/music_and_sound_g.h"

#include "g_src/renderer_curses.hpp"
#include "g_src/renderer_2d.hpp"
#include "g_src/renderer_offscreen.hpp"
#include "g_src/renderer_opengl.hpp"

void enablerst::async_loop() {
  async_paused = false;
  async_frames = 0;
  int fps = 100; // Just a thread-local copy
  for (;;) {
    // cout << "FRAMES: " << frames << endl;
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
            // puts("Paused");
            async_frombox.write(async_msg(async_msg::complete));
            break;
          case async_cmd::start:
            async_paused = false;
            async_frames = 0;
            // puts("UNpaused");
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

void enablerst::eventLoop_SDL() {

  SDL_Event event;
  const SDL_Surface *screen = SDL_GetVideoSurface();
  Uint32 mouse_lastused = 0;
  SDL_ShowCursor(SDL_DISABLE);

  // Initialize the grid
  renderer->resize(screen->w, screen->h);

  while (loopvar) {
    Uint32 now = SDL_GetTicks();
    bool paused_loop = false;

    // Check for zoom commands
    zoom_commands zoom;
    while (async_zoom.try_read(zoom)) {
      if (overridden_grid_sizes.size())
        continue; // No zooming in movies
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      if (zoom == zoom_fullscreen)
        renderer->set_fullscreen();
      else
        renderer->zoom(zoom);
    }

    // Check for SDL events
    while (SDL_PollEvent(&event)) {
      // Make sure mainloop isn't running while we're processing input
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      // Handle SDL events
      switch (event.type) {
        case SDL_KEYDOWN:
          // Disable mouse if it's been long enough
          if (mouse_lastused + 5000 < now) {
            if (init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
              // hide the mouse picture
              // enabler.set_tile(0, TEXTURE_MOUSE, enabler.mouse_x, enabler.mouse_y);
            }
            SDL_ShowCursor(SDL_DISABLE);
          }
        case SDL_KEYUP:
        case SDL_QUIT:
          enabler.add_input(event, now);
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          if (!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
            int isdown = (event.type == SDL_MOUSEBUTTONDOWN);
            if (event.button.button == SDL_BUTTON_LEFT) {
              enabler.mouse_lbut = isdown;
              enabler.mouse_lbut_down = isdown;
              if (!isdown)
                enabler.mouse_lbut_lift = 0;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
              enabler.mouse_rbut = isdown;
              enabler.mouse_rbut_down = isdown;
              if (!isdown)
                enabler.mouse_rbut_lift = 0;
            } else
              enabler.add_input(event, now);
          }
          break;
        case SDL_MOUSEMOTION:
          // Deal with the mouse hiding bit
          mouse_lastused = now;
          if (init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_PICTURE)) {
            // turn on mouse picture
            // enabler.set_tile(gps.tex_pos[TEXTURE_MOUSE], TEXTURE_MOUSE,enabler.mouse_x, enabler.mouse_y);
          } else {
            SDL_ShowCursor(SDL_ENABLE);
          }
          break;
        case SDL_ACTIVEEVENT:
          enabler.clear_input();
          if (event.active.state & SDL_APPACTIVE) {
            if (event.active.gain) {
              enabler.flag |= ENABLERFLAG_RENDER;
              gps.force_full_display_count++;
            }
          }
          break;
        case SDL_VIDEOEXPOSE:
          gps.force_full_display_count++;
          enabler.flag |= ENABLERFLAG_RENDER;
          break;
        case SDL_VIDEORESIZE:
          if (is_fullscreen())
            ;
          //errorlog << "Caught resize event in fullscreen??\n";
          else {
            //gamelog << "Resizing window to " << event.resize.w << "x" << event.resize.h << endl << flush;
            renderer->resize(event.resize.w, event.resize.h);
          }
          break;
      } // switch (event.type)
    } //while have event

    // Update mouse state
    if (!init.input.flag.has_flag(INIT_INPUT_FLAG_MOUSE_OFF)) {
      int mouse_x = -1, mouse_y = -1, mouse_state;
      // Check whether the renderer considers this valid input or not, and write it to gps
      if ((SDL_GetAppState() & SDL_APPMOUSEFOCUS) && renderer->get_mouse_coords(mouse_x, mouse_y)) {
        mouse_state = 1;
      } else {
        mouse_state = 0;
      }
      if (mouse_x != gps.mouse_x || mouse_y != gps.mouse_y || mouse_state != enabler.tracking_on) {
        // Pause rendering loop and update values
        if (!paused_loop) {
          pause_async_loop();
          paused_loop = true;
        }
        enabler.tracking_on = mouse_state;
        gps.mouse_x = mouse_x;
        gps.mouse_y = mouse_y;
      }
    }

    if (paused_loop)
      unpause_async_loop();

    do_frame();
#if !defined(NO_FMOD)
    // Call FMOD::System.update(). Manages a bunch of sound stuff.
    musicsound.update();
#endif
  }
}

int enablerst::loop(string cmdline) {
  command_line = cmdline;

  // Initialize the tick counters
  simticks.write(0);
  gputicks.write(0);

  // Call DF's initialization routine
  if (!beginroutine())
    exit(EXIT_FAILURE);

  // Allocate a renderer
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
#ifdef CURSES
    renderer = new renderer_curses();
#else
    report_error("PRINT_MODE","TEXT not supported on windows");
    exit(EXIT_FAILURE);
#endif
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_2D)) {
    renderer = new renderer_2d();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER)) {
    renderer = new renderer_accum_buffer();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER)) {
    renderer = new renderer_framebuffer();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT)) {
    if (init.display.partial_print_count)
      renderer = new renderer_partial();
    else
      renderer = new renderer_once();
  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_VBO)) {
    renderer = new renderer_vbo();
  } else {
    renderer = new renderer_opengl();
  }

  // At this point we should have a window that is setup to render DF.
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
#ifdef CURSES
    eventLoop_ncurses();
#endif
  } else {
    SDL_EnableUNICODE(1);
    eventLoop_SDL();
  }

  endroutine();

  // Clean up graphical resources
  delete renderer;
}

