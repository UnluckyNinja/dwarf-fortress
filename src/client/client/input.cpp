/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "input.hpp"

#include "signals.hpp"
#include "message.hpp"
#include "message/input.hpp"
#include "message/internal.hpp"
#include "context.hpp"

#include <set>

#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>

#ifndef __x86_64__
#include "g_src/enabler.h"
#endif

namespace df {

  input_client::input_client(df::client_configuration_t const& config) :
      config_(config) {
  }

  void input_client::input(zmq::context_t& zmq_context) {
    zmq::socket_t zmq_publisher(zmq_context, ZMQ_PUB);
    std::string const zmq_address = config_.connection_protocol + "://" + config_.server_name + ":"
        + config_.input_port;
    zmq_publisher.connect(zmq_address.c_str());

    /* Notify waiting threads that we are ready to receive input control */
    zmq::socket_t zmq_input(zmq_context, ZMQ_REP);
    zmq_input.connect("inproc://input");
    {
      df::bytes request;
      df::receive(zmq_input, request, true);

      df::bytes response;
      df::pack(response, df::message::internal_t(df::message::internal::synchronization_response));
      df::send(zmq_input, request, false);
    }

    SDL_EnableUNICODE(1);

    while (!df::kill_received()) {

      Uint32 now = SDL_GetTicks();
      SDL_Event event;
      while (SDL_PollEvent(&event)) {

        // Handle SDL events
        switch (event.type) {
          case SDL_KEYDOWN:
          case SDL_KEYUP:
          case SDL_QUIT:
#ifndef __x86_64__
            enabler.add_input(event, now);
#endif
            break;

          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
            break;

          case SDL_MOUSEMOTION:
            break;

          case SDL_ACTIVEEVENT:
#ifndef __x86_64__
            enabler.clear_input();
#endif
            break;

          case SDL_VIDEOEXPOSE:
            break;

          case SDL_VIDEORESIZE: {
            df::display::release();

            // Receive window resize request, and ask for synchronization.
            df::bytes request;
            if (df::receive(zmq_input, request, true)) {
              df::bytes bytes;
              df::message::internal_t response(df::message::internal::synchronization_request);

              df::pack(bytes, response);
              df::send(zmq_input, bytes, false);
            }

            // Receive window resize request, and send window resize response.
            if (df::receive(zmq_input, request, true)) {
              SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_HWSURFACE | SDL_RESIZABLE);

              df::bytes bytes;
              df::message::internal_t response(df::message::internal::window_resize_response);
              response.width = event.resize.w;
              response.height = event.resize.h;

              df::pack(bytes, response);
              df::send(zmq_input, bytes, true);
            }

            break;
          }

        }
      }

      std::string peer_uuid = config_.client_uuid;
      std::set< std::uint64_t > inputs;
#ifndef __x86_64__
      for (InterfaceKey key : enabler.get_input(now)) {
        inputs.insert(key);
      }
#endif

      if (inputs.size() > 0) {
        df::message::input_t message;
        message.count = inputs.size();
        message.keys.assign(inputs.begin(), inputs.end());
        df::send(zmq_publisher, peer_uuid, message);
      }

    }
  }

  input_client::control_thread::control_thread(input_client& client, zmq::context_t& zmq_context) :
      client_(client), zmq_context_(zmq_context) {
  }

  void input_client::control_thread::operator()() {
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);
    SDL_InitSubSystem(SDL_INIT_VIDEO);

#ifndef __x86_64__
    keybinding_init();
    enabler.load_keybindings("data/init/interface.txt");
#endif

    // Disable key repeat
    SDL_EnableKeyRepeat(0, 0);

    // Set window title/icon.
    SDL_WM_SetCaption("Dwarf Fortress", NULL);
    SDL_Surface* icon = IMG_Load("data/art/icon.png");
    if (icon != NULL) {
      SDL_WM_SetIcon(icon, NULL);
      SDL_FreeSurface(icon);
    }

    SDL_SetVideoMode(500, 500, 32, SDL_HWSURFACE | SDL_RESIZABLE);
    SDL_ShowCursor(SDL_DISABLE);

    df::display::release();
    client_.input(zmq_context_);

    SDL_Quit();
  }

} // namespace df
