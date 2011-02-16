/**
 * @file
 * @date 28 janv. 2011
 * @todo comment
 */

#ifndef INTERN_CONFIG_HPP_
#define INTERN_CONFIG_HPP_

#include "meta/singleton.hpp"

#include <cstdint>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

namespace bpo = ::boost::program_options;

struct texture_config {
    ::std::string small_font_filename;
    ::std::string large_font_filename;
    bool use_true_type;
    bool use_graphics;
    bool add_black_spaces;
    bool use_linear_filtering;
    bool show_mouse;
};

struct window_config {
    ::std::uint32_t window_width;
    ::std::uint32_t window_height;
    bool window_not_resizable;
    bool fullscreen;
    ::std::uint32_t fullscreen_width;
    ::std::uint32_t fullscreen_height;
    bool keep_topmost;
};

struct display_config {
    enum display_mode {
      text, software_2d, hardware_2d, asynchronous_2d, framebuffer, accumulation_buffer, vertex_buffer_object, shader, standard
    };

    display_mode mode;
    bool use_partial_print;
    ::std::uint32_t partial_print_count;

    bool use_vsync;
    bool use_arb_sync;
    bool use_single_buffer;

    bool show_fps;
    ::std::uint32_t fps_cap;
};

struct input_config {
    ::std::uint32_t zoom_speed;
    bool disable_mouse;
    ::std::uint32_t key_hold_delay;
    ::std::uint32_t key_repeat_delay;
    ::std::uint32_t key_repeat_acceleration_limit;
    ::std::uint32_t key_repeat_acceleration_start;
    ::std::uint32_t macro_step_delay;
    ::std::uint32_t recenter_interface_delay;
};

struct gameplay_config {
    ::std::uint32_t fps_cap;
    bool disable_intro;
    bool disable_music;
    ::std::uint32_t music_volume;
    bool compress_saves;
};

struct world_config {
    bool generate;
    ::std::uint64_t id;
    ::std::uint64_t seed;
    ::std::string parameter;
    bool use_seed;
    bool use_parameter;
};

struct config: public singleton< config > {
    void init(int arg_count, char** arg_values);

    texture_config const& texture() const;
    window_config const& window() const;
    display_config const& display() const;
    input_config const& input() const;
    gameplay_config const& gameplay() const;
    world_config const& world() const;

  private:
    texture_config __texture;
    window_config __window;
    display_config __display;
    input_config __input;
    gameplay_config __gameplay;
    world_config __world;
};

#endif /* INTERN_CONFIG_HPP_ */
