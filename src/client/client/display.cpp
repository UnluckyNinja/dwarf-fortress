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

#define GL3_PROTOTYPES
#include <GL3/gl3.h>

#include <SDL/SDL_video.h>

namespace df {

  display_client::display_client(df::client_configuration_t const& config) :
      config_(config) {
  }

  static void create_program(std::uint32_t& program) {
    std::int32_t result;
    static char const vertex_shader_source[] = ""
        "#version 330 core\n"
        "\n"
        "uniform vec2 dimensions;\n"
        "\n"
        "in vec2 position;\n"
        "out vert {\n"
        "  vec2 coordinates;\n"
        "} vertex;\n"
        "\n"
        "void main() {\n"
        "  gl_Position = vec4(position, 0.0, 1.0);\n"
        "  vertex.coordinates.x = dimensions.x * position.x;\n"
        "  vertex.coordinates.y = dimensions.y * position.y;\n"
        "}";
    static char const* vertex_shader_source_ptr = vertex_shader_source;
    static std::int32_t const vertex_shader_source_len = sizeof(vertex_shader_source);
    std::uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source_ptr, &vertex_shader_source_len);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    if (!result) {
      char buffer[2048];
      std::int32_t log_length;
      glGetShaderInfoLog(vertex_shader, 2048, &log_length, buffer);
      std::clog << "vertex shader:\n" << std::string(buffer, log_length) << "\n";
    }

    static char const fragment_shader_source[] = ""
        "#version 330 core\n"
        "\n"
        "uniform vec2 dimensions;\n"
        "\n"
        "uniform uint current_frame;\n"
        "uniform usampler2D update_time;\n"
        "uniform usampler2D character;\n"
        "uniform usampler2D character_foreground;\n"
        "uniform usampler2D character_background;\n"
        "uniform usampler2D character_bold;\n"
        "\n"
        "uniform usampler2D tile;\n"
        "uniform usampler2D tile_color_character;\n"
        "uniform usampler2D tile_color_colorize;\n"
        "uniform usampler2D tile_foreground;\n"
        "uniform usampler2D tile_background;\n"
        "\n"
        "uniform uvec2 tile_size;\n"
        "uniform sampler3D tileset;\n"
        "\n"
        "in vert {\n"
        "  vec2 coordinates;\n"
        "} vertex;\n"
        "\n"
        "out vec4 output_image;\n"
        "\n"
        "void main() {\n"
        "  output_image = texture(update_time, vertex.coordinates);\n"
        "}\n"
        "";
    static char const* fragment_shader_source_ptr = fragment_shader_source;
    static std::int32_t const fragment_shader_source_len = sizeof(fragment_shader_source);
    std::uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source_ptr, &fragment_shader_source_len);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    if (!result) {
      char buffer[2048];
      std::int32_t log_length;
      glGetShaderInfoLog(fragment_shader, 2048, &log_length, buffer);
      std::clog << "fragment shader:\n" << std::string(buffer, log_length) << "\n";
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (!result) {
      char buffer[2048];
      std::int32_t log_length;
      glGetProgramInfoLog(program, 2048, &log_length, buffer);
      std::clog << "program:\n" << std::string(buffer, log_length) << "\n";
    }

    glUseProgram(program);
  }

  static void create_textures(std::uint32_t program) {
    std::uint32_t update_time;
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

    std::uint32_t program;
    create_program(program);

    std::uint32_t position = glGetAttribLocation(program, "position");
    std::uint32_t dimensions = glGetUniformLocation(program, "dimensions");
    std::uint32_t current_frame = glGetUniformLocation(program, "current_frame");
    std::uint32_t update_time = glGetUniformLocation(program, "update_time");

    std::uint32_t character = glGetUniformLocation(program, "character");
    std::uint32_t character_foreground = glGetUniformLocation(program, "character_foreground");
    std::uint32_t character_background = glGetUniformLocation(program, "character_background");
    std::uint32_t character_bold = glGetUniformLocation(program, "character_bold");
    std::uint32_t tile = glGetUniformLocation(program, "tile");
    std::uint32_t tile_color_character = glGetUniformLocation(program, "tile_color_character");
    std::uint32_t tile_color_colorize = glGetUniformLocation(program, "tile_color_colorize");
    std::uint32_t tile_foreground = glGetUniformLocation(program, "tile_foreground");
    std::uint32_t tile_background = glGetUniformLocation(program, "tile_background");
    std::uint32_t tile_size = glGetUniformLocation(program, "tile_size");
    std::uint32_t tileset = glGetUniformLocation(program, "tileset");

    std::int8_t const positions_data[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
    std::uint32_t position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, position_buffer);
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(positions_data), positions_data, GL_STATIC_DRAW);

    std::uint8_t const indexes_data[] = { 0, 1, 2, 3 };
    std::uint32_t indexes_buffer;
    glGenBuffers(1, &indexes_buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, indexes_buffer);
    glBufferData(GL_COPY_WRITE_BUFFER, sizeof(indexes_data), indexes_data, GL_STATIC_DRAW);

    std::uint32_t vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glVertexAttribPointer(position, 2, GL_BYTE, true, 0, 0);
    glEnableVertexAttribArray(position);

    std::uint32_t update_time_texture;
    glGenTextures(1, &update_time_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, update_time_texture);
    glUniform1i(update_time, 0);

    glViewport(0, 0, 500, 500);
    glDepthRangef(0, 10);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexes_buffer);

    std::uint32_t frame = 0;
    while (!df::kill_received()) {
      frame++;

      std::string server_uuid;
      while (df::receive(zmq_listener, server_uuid, message)) {

        switch (message.type) {
          case df::message::display::screen_update: {
            if (message.resets_bounds) {
//              std::cerr << "client::grid_resize(" << message.width << "," << message.height << ") not implemented.\n";
//              renderer->grid_resize(message.width, message.height);
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, message.width, message.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
              glUniform2i(dimensions, message.width, message.height);
            }

            df::bytes data(message.height * message.width * sizeof(std::uint32_t), frame);
            glTexSubImage2D(GL_TEXTURE_2D,
                            0,
                            message.x,
                            message.y,
                            message.width,
                            message.height,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            data.data());

//            for (std::uint32_t x = 0; x < message.width; ++x) {
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
//            }
            break;
          }

          case df::message::display::tileset_info:
            break;
        }

      }

      glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, 0);
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
