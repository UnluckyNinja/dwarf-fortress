/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "display.hpp"

#include "signals.hpp"
#include "message.hpp"
#include "message/display.hpp"
#include "message/internal.hpp"
#include "context.hpp"

#include <SDL/SDL_video.h>

#define GTULU_USE_LIBLOGGING
#include "client_program_format.hpp"
#include "gtulu/internal/storage/data.hpp"

namespace df {

  display_client::display_client(df::client_configuration_t const& config) :
      config_(config) {
  }

  void display_client::display(zmq::context_t& zmq_context) {
    zmq::socket_t zmq_listener(zmq_context, ZMQ_SUB);
    std::string const zmq_address = config_.connection_protocol + "://" + config_.server_name + ":"
        + config_.display_port;
    zmq_listener.connect(zmq_address.c_str());

    df::message::display_t message;
    df::subscribe(zmq_listener, message);

    /* Notify waiting threads that we are ready to receive display control */
    zmq::socket_t zmq_display(zmq_context, ZMQ_REP);
    zmq_display.connect("inproc://display");
    {
      df::bytes request;
      df::receive(zmq_display, request, true);

      df::bytes response;
      df::pack(response, df::message::internal_t(df::message::internal::synchronization_response));
      df::send(zmq_display, request, false);
    }

    using namespace gtulu::internal;
    float const positions_data[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
    std::uint8_t const indexes_data[] = { 0, 1, 2, 3 };

    obj::buffer< fdat::gl_float > positions(positions_data);
    obj::buffer< fdat::gl_unsigned_byte > indexes(indexes_data);

    obj::program< fprg::client_program_format > program;
    auto framebuffer = program.get_default_framebuffer();
    auto vertexarray = program.new_vertexarray();

    // Bind the vertex attribute named "position" to the positions buffer and "texture_position" to the texture_positions buffer.
    vertexarray->set_position(positions);

    // Select default texture format for 2d texture.
    typedef ftex::select_format< ftgt::gl_texture_2d, fcmn::component::red_green_blue_alpha, fnum::ufixed8_ >::type texture_format;
    df::bytes zero(128 * 128 * sizeof(std::uint8_t) * 4, 0);
    obj::texture< texture_format > last_update(sto::wrap(zero.data(), 128, 128));

    obj::texture< texture_format > characters(sto::wrap(zero.data(), 128, 128));

    // Bind the uniform sampler named "background" to texture.
    program.set_last_update(last_update);
    program.set_characters(characters);

    framebuffer->set_viewport(400, 400, 10, 50, 50, 0);
    program.set_width(80);
    program.set_height(25);

    std::uint8_t frame = 0;
    while (!df::kill_received()) {
      frame++;
      program.set_last_frame(static_cast< float >(frame) / 255);

      std::string server_uuid;
      if (df::receive(zmq_listener, server_uuid, message)) {

        switch (message.type) {
          case df::message::display::screen_update: {
            if (message.resets_bounds) {
              std::cerr << "client::grid_resize(" << message.width << "," << message.height << ") not implemented.\n";
//              renderer->grid_resize(message.width, message.height);
              program.set_width(message.width);
              program.set_height(message.height);
            }

            df::bytes bytes(message.width * message.height * 4 * sizeof(frame), frame);
            auto range = sto::range_of(last_update, sto::data::offset_type(message.y, message.x));
            sto::copy(range, sto::wrap(bytes.data(), message.height, message.width));

            auto char_range = sto::range_of(characters, sto::data::offset_type(message.y, message.x));
            sto::copy(char_range,
                      sto::wrap(reinterpret_cast< uint8_t* >(message.characters.data()),
                                message.height,
                                message.width));

//            auto char_range = sto::range_of(characters, sto::data::offset_type(message.x, message.y));
//            sto::copy(char_range, sto::wrap(message.characters.data(), message.width, message.height));

            for (std::uint32_t x = 0; x < message.width; ++x) {
//              df::graphic_character_t* characters = reinterpret_cast< df::graphic_character_t* >(gps.screen)
//                  + (message.x + x) * gps.dimy + message.y;
//              memcpy(characters,
//                     message.characters.data() + x * message.height,
//                     message.height * sizeof(df::graphic_character_t));
//
//              df::texture_index_t* texture_indexes = reinterpret_cast< df::texture_index_t* >(gps.screentexpos)
//                  + (message.x + x) * gps.dimy + message.y;
//              memcpy(texture_indexes,
//                     message.texture_indexes.data() + x * message.height,
//                     message.height * sizeof(df::texture_index_t));
//
//              for (std::uint32_t y = 0; y < message.height; ++y) {
//                std::uint32_t t = (message.x + x) * gps.dimy + (message.y + y);
//                df::texture_color_t c = message.texture_colors[x * message.height + y];
//
//                gps.screentexpos_addcolor[t] = (c >> 24) & 0xFF;
//                gps.screentexpos_grayscale[t] = (c >> 16) & 0xFF;
//                gps.screentexpos_cf[t] = (c >> 8) & 0xFF;
//                gps.screentexpos_cbr[t] = (c >> 0) & 0xFF;
//              }
            }
            break;
          }

          case df::message::display::tileset_info:
            break;
        }

      } else {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
      }

      vertexarray->draw< drw::mode::gl_triangle_strip >(program, *framebuffer, indexes, 4);
      SDL_GL_SwapBuffers();
    }
  }

  display_client::control_thread::control_thread(display_client& client, zmq::context_t& zmq_context) :
      client_(client), zmq_context_(zmq_context) {
  }

  void display_client::control_thread::operator()() {
    /* Wait for input thread to create the window */
    zmq::socket_t zmq_input(zmq_context_, ZMQ_REQ);
    zmq_input.bind("inproc://input");
    {
      df::bytes request;
      df::pack(request, df::message::internal_t(df::message::internal::synchronization_request));
      df::send(zmq_input, request, true);

      df::bytes response;
      df::receive(zmq_input, response, true);
    }

    do {
    } while (!df::display::acquire());

    client_.display(zmq_context_);
  }

} // namespace df
