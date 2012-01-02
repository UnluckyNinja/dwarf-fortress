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
#include "gtulu/internal/context.hpp"
#include "gtulu/internal/storage/data.hpp"

namespace df {

  display_client::display_client(df::client_configuration_t const& config) :
      config_(config) {
  }

  void display_client::display(zmq::context_t& zmq_context) {
    /* Wait for input thread to create the window */
    zmq::socket_t zmq_input(zmq_context, ZMQ_REQ);
    zmq_input.bind("inproc://input");
    {
      df::bytes request;
      df::pack(request, df::message::internal_t(df::message::internal::synchronization_request));
      df::send(zmq_input, request, true);

      df::bytes response;
      df::receive(zmq_input, response, true);

      request.clear();
      df::pack(request, df::message::internal_t(df::message::internal::window_resize_request));
      df::send(zmq_input, request);
    }

    do {
    } while (!df::display::acquire());

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

    std::string const gl_vendor = ctx::gl_vendor::get();
    std::string const gl_renderer = ctx::gl_renderer::get();
    std::string const gl_version = ctx::gl_version::get();
    std::string const gl_shading_language_version = ctx::gl_shading_language_version::get();

    __gtulu_info() << ctx::parameter::gl_vendor() << ": " << gl_vendor;
    __gtulu_info() << ctx::parameter::gl_renderer() << ": " << gl_renderer;
    __gtulu_info() << ctx::parameter::gl_version() << ": " << gl_version;
    __gtulu_info() << ctx::parameter::gl_shading_language_version() << ": " << gl_shading_language_version;

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
    typedef ftex::select_format< ftgt::gl_texture_2d, fcmn::component::red, fnum::uint32_ >::type utexture_format;
    df::bytes zero(128 * 128 * sizeof(std::uint8_t) * 4, 0);
    obj::texture< utexture_format > last_update(sto::wrap(zero.data(), 128, 128));

    glBindTexture(GL_TEXTURE_2D, *last_update);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    obj::texture< texture_format > characters(sto::wrap(zero.data(), 128, 128));
    obj::texture< texture_format > texture_colors(sto::wrap(zero.data(), 128, 128));
    obj::texture< texture_format > texture_indexes(sto::wrap(zero.data(), 128, 128));

    framebuffer->set_viewport(500, 500, 10, 0, 0, 0);
    program.set_width(80);
    program.set_height(25);

    std::uint32_t frame = 0;
    while (!df::kill_received()) {
      frame++;
      program.set_last_frame(frame);

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

            program.set_last_update(last_update);
            std::vector< std::uint32_t > bytes(message.width * message.height, frame);
            auto range = sto::at(last_update, sto::data::offset_type(message.y, message.x));
            sto::copy(range, sto::wrap(reinterpret_cast< uint8_t* >(bytes.data()), message.height, message.width));

            program.set_characters(characters);
            auto char_range = sto::at(characters, sto::data::offset_type(message.y, message.x));
            sto::copy(char_range,
                      sto::wrap(reinterpret_cast< uint8_t* >(message.characters.data()),
                                message.height,
                                message.width));

            program.set_texture_colors(texture_colors);
            auto tex_range = sto::at(texture_colors, sto::data::offset_type(message.y, message.x));
            sto::copy(tex_range,
                      sto::wrap(reinterpret_cast< uint8_t* >(message.texture_colors.data()),
                                message.height,
                                message.width));

            program.set_texture_indexes(texture_indexes);
            auto texi_range = sto::at(texture_indexes, sto::data::offset_type(message.y, message.x));
            sto::copy(texi_range,
                      sto::wrap(reinterpret_cast< uint8_t* >(message.texture_indexes.data()),
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
        df::bytes response;
        while (df::receive(zmq_input, response)) {
          df::message::internal_t internal_response(df::message::internal::type::window_resize_response);
          df::unpack(response, internal_response);

          switch (internal_response.type_) {
            case df::message::internal::type::window_resize_response:
              framebuffer->set_viewport(internal_response.width, internal_response.height, 10, 0, 0, 0);
              break;
            default:
              break;
          }

          df::bytes request;
          df::pack(request, df::message::internal_t(df::message::internal::window_resize_request));
          df::send(zmq_input, request);
        }

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
    client_.display(zmq_context_);
  }

} // namespace df
