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

#include "client_program_format.hpp"
#include "gtulu/internal/context.hpp"
#include "gtulu/internal/storage/data.hpp"
#include "gtulu/internal/storage/copy.hpp"

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
    df::display::acquire();

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

    typedef ftex::select_format< ftgt::gl_texture_2d_array, fcmn::component::red_green_blue_alpha, fnum::ufixed8_ >::type tileset_format;
    typedef ftex::select_format< ftgt::gl_texture_1d, fcmn::component::red_green_blue, fnum::uint8_ >::type tileset_info_format;
    df::bytes zero(128 * 128 * sizeof(std::uint8_t) * 4, 0);

    obj::texture< texture_format > characters(sto::wrap(zero.data(), 128, 128));
    obj::texture< texture_format > texture_colors(sto::wrap(zero.data(), 128, 128));
    obj::texture< texture_format > texture_indexes(sto::wrap(zero.data(), 128, 128));

    obj::texture< tileset_format > tilesets;
    obj::texture< tileset_info_format > tilesets_info;

    glBindTexture(GL_TEXTURE_1D, *tilesets_info);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    framebuffer->set_viewport(500, 500, 10, 0, 0, 0);
    program.set_grid_size(80, 25);
//    program.set_window_size(500, 500);

    std::uint32_t frame = 0;
    while (!df::kill_received()) {
      frame++;
//      program.set_last_frame(frame);

      std::string server_uuid;
      if (df::receive(zmq_listener, server_uuid, message)) {

        switch (message.type) {
          case df::message::display::screen_update: {
            if (message.resets_bounds) {
              std::cerr << "client::grid_resize(" << message.width << "," << message.height << ") not implemented.\n";
              program.set_grid_size(message.width, message.height);
            }

            program.set_characters(characters);
            sto::copy(sto::at(characters, message.y, message.x),
                      sto::wrap(reinterpret_cast< uint8_t* >(message.characters.data()),
                                message.height,
                                message.width));

            program.set_texture_indexes(texture_indexes);
            sto::copy(sto::at(texture_indexes, message.y, message.x),
                      sto::wrap(reinterpret_cast< uint8_t* >(message.texture_indexes.data()),
                                message.height,
                                message.width));

            program.set_texture_colors(texture_colors);
            sto::copy(sto::at(texture_colors, message.y, message.x),
                      sto::wrap(reinterpret_cast< uint8_t* >(message.texture_colors.data()),
                                message.height,
                                message.width));
            break;
          }

          case df::message::display::tileset_info:
            std::uint32_t depth = 0;
            std::uint32_t width = 0;
            std::uint32_t height = 0;

            std::vector< std::uint8_t > tilesets_info_data;
            for (df::message::tileset_t const& tileset : message.tilesets) {
              width = std::max(width, tileset.tile_count_x_ * tileset.tile_size_x_);
              height = std::max(height, tileset.tile_count_y_ * tileset.tile_size_y_);
              depth = std::max(depth, tileset.tileset_id_ + 1u);
            }

            width = 1 << (static_cast< std::uint8_t >(log2(width)) + 1);
            height = 1 << (static_cast< std::uint8_t >(log2(height)) + 1);
            depth = 1 << (static_cast< std::uint8_t >(log2(depth)) + 1);

            tilesets.resize(width, height, depth);
            tilesets_info.resize(depth);
            tilesets_info_data.resize(3 * depth);

            program.set_tilesets(tilesets);
            for (df::message::tileset_t const& tileset : message.tilesets) {
              std::uint32_t const tileset_width = tileset.tile_count_x_ * tileset.tile_size_x_;
              std::uint32_t const tileset_height = tileset.tile_count_y_ * tileset.tile_size_y_;
              sto::copy(sto::at(tilesets, 0, 0, tileset.tileset_id_),
                        sto::wrap(reinterpret_cast< uint8_t const* >(tileset.pixels.data()),
                                  tileset_width,
                                  tileset_height));

              std::cerr << tileset.tile_size_x_ << "x" << tileset.tile_size_y_ << "\n";

              tilesets_info_data[tileset.tileset_id_ * 3] = tileset.tile_size_x_;
              tilesets_info_data[tileset.tileset_id_ * 3 + 1] = tileset.tile_size_y_;
              tilesets_info_data[tileset.tileset_id_ * 3 + 2] = 0;
            }

            program.set_tilesets_info(tilesets_info);
            sto::copy(tilesets_info, sto::wrap(reinterpret_cast< uint8_t const* >(tilesets_info_data.data()), depth));

            break;
        }

        vertexarray->draw< drw::mode::gl_triangle_strip >(program, *framebuffer, indexes, 4);
        df::display::swap_buffers();

      } else {

        df::bytes response;
        // Now wait for the window resize response
        if (df::receive(zmq_input, response, true)) {
          df::message::internal_t internal_response(df::message::internal::window_resize_response);
          df::unpack(response, internal_response);
          framebuffer->set_viewport(internal_response.width, internal_response.height, 10, 0, 0, 0);

          df::bytes request;
          df::pack(request, df::message::internal_t(df::message::internal::window_resize_request));
          df::send(zmq_input, request);

        } else {
          boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }

      }
    }
  }

  display_client::control_thread::control_thread(display_client& client, zmq::context_t& zmq_context) :
      client_(client), zmq_context_(zmq_context) {
  }

  void display_client::control_thread::operator()() {
    client_.display(zmq_context_);
    df::display::release();
  }

} // namespace df
