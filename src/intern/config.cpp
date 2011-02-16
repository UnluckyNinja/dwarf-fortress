/**
 * @file
 * @date 28 janv. 2011
 * @todo comment
 */

#include "intern/config.hpp"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

void validate(::boost::any& value_out, ::std::vector< ::std::string > const& values_in, display_config::display_mode*, int) {
  bpo::validators::check_first_occurrence(value_out);
  ::std::string const value = ::boost::to_lower_copy(bpo::validators::get_single_string(values_in));

  if (value.compare("text") == 0) {
    value_out = ::boost::any(display_config::display_mode::text);
  } else if (value.compare("shader") == 0) {
    value_out = ::boost::any(display_config::display_mode::shader);
  } else if (value.compare("vertex_buffer_object") == 0) {
    value_out = ::boost::any(display_config::display_mode::vertex_buffer_object);
  } else if (value.compare("accumulation_buffer") == 0) {
    value_out = ::boost::any(display_config::display_mode::accumulation_buffer);
  } else if (value.compare("framebuffer") == 0) {
    value_out = ::boost::any(display_config::display_mode::framebuffer);
  } else if (value.compare("asynchronous_2d") == 0) {
    value_out = ::boost::any(display_config::display_mode::asynchronous_2d);
  } else if (value.compare("hardware_2d") == 0) {
    value_out = ::boost::any(display_config::display_mode::hardware_2d);
  } else if (value.compare("software_2d") == 0) {
    value_out = ::boost::any(display_config::display_mode::software_2d);
  } else if (value.compare("standard") == 0) {
    value_out = ::boost::any(display_config::display_mode::standard);
  } else {
    throw bpo::validation_error(bpo::validation_error::invalid_option_value, value, "display.mode");
  }
}

static void set_seed(world_config& world_inout, ::std::uint64_t const& seed_in) {
  world_inout.use_seed = true;
  world_inout.seed = seed_in;
}

static void set_parameter(world_config& world_inout, ::std::string const& parameter_in) {
  world_inout.use_parameter = true;
  world_inout.parameter = parameter_in;
}

texture_config const& config::texture() const {
  return __texture;
}
window_config const& config::window() const {
  return __window;
}
display_config const& config::display() const {
  return __display;
}
input_config const& config::input() const {
  return __input;
}
gameplay_config const& config::gameplay() const {
  return __gameplay;
}
world_config const& config::world() const {
  return __world;
}

void config::init(int arg_count, char** arg_values) {
  bpo::options_description config_file_options;
  bpo::options_description command_line_options;
  bpo::options_description_easy_init add_option = config_file_options.add_options();

  bpo::options_description texture_options("Texture Options", 120, 60);
  add_option = texture_options.add_options();

  add_option("texture.small_font_filename", bpo::value(&__texture.small_font_filename)->required(), "@todo: description");
  add_option("texture.large_font_filename", bpo::value(&__texture.large_font_filename)->required(), "@todo: description");
  add_option("texture.use_true_type", bpo::bool_switch(&__texture.use_true_type), "@todo: description");
  add_option("texture.use_graphics", bpo::bool_switch(&__texture.use_graphics), "@todo: description");
  add_option("texture.add_black_spaces", bpo::bool_switch(&__texture.add_black_spaces), "@todo: description");
  add_option("texture.use_linear_filtering", bpo::bool_switch(&__texture.use_linear_filtering), "@todo: description");
  add_option("texture.show_mouse", bpo::bool_switch(&__texture.show_mouse), "@todo: description");

  bpo::options_description window_options("Window Options", 120, 60);
  add_option = window_options.add_options();
  add_option("window.window_width", bpo::value(&__window.window_width)->required(), "@todo: description");
  add_option("window.window_height", bpo::value(&__window.window_height)->required(), "@todo: description");
  add_option("window.window_not_resizable", bpo::bool_switch(&__window.window_not_resizable), "@todo: description");
  add_option("window.fullscreen", bpo::bool_switch(&__window.fullscreen), "@todo: description");
  add_option("window.fullscreen_width", bpo::value(&__window.fullscreen_width), "@todo: description");
  add_option("window.fullscreen_height", bpo::value(&__window.fullscreen_height), "@todo: description");
  add_option("window.keep_topmost", bpo::bool_switch(&__window.keep_topmost), "@todo: description");

  bpo::options_description display_options("Display Options", 120, 60);
  add_option = display_options.add_options();
  add_option("display.mode", bpo::value(&__display.mode)->required(), "@todo: description");
  add_option("display.use_partial_print", bpo::bool_switch(&__display.use_partial_print), "@todo: description");
  add_option("display.partial_print_count", bpo::value(&__display.partial_print_count), "@todo: description");
  add_option("display.use_vsync", bpo::bool_switch(&__display.use_vsync), "@todo: description");
  add_option("display.use_arb_sync", bpo::bool_switch(&__display.use_arb_sync), "@todo: description");
  add_option("display.use_single_buffer", bpo::bool_switch(&__display.use_single_buffer), "@todo: description");
  add_option("display.show_fps", bpo::bool_switch(&__display.show_fps), "@todo: description");
  add_option("display.fps_cap", bpo::value(&__display.fps_cap), "@todo: description");

  bpo::options_description input_options("Input Options", 120, 60);
  add_option = input_options.add_options();
  add_option("input.zoom_speed", bpo::value(&__input.zoom_speed), "@todo: description");
  add_option("input.disable_mouse", bpo::bool_switch(&__input.disable_mouse), "@todo: description");
  add_option("input.key_hold_delay", bpo::value(&__input.key_hold_delay), "@todo: description");
  add_option("input.key_repeat_delay", bpo::value(&__input.key_repeat_delay), "@todo: description");
  add_option("input.key_repeat_acceleration_limit", bpo::value(&__input.key_repeat_acceleration_limit), "@todo: description");
  add_option("input.key_repeat_acceleration_start", bpo::value(&__input.key_repeat_acceleration_start), "@todo: description");
  add_option("input.macro_step_delay", bpo::value(&__input.macro_step_delay), "@todo: description");
  add_option("input.recenter_interface_delay", bpo::value(&__input.recenter_interface_delay), "@todo: description");

  bpo::options_description gameplay_options("Gameplay Options", 120, 60);
  add_option = gameplay_options.add_options();
  add_option("gameplay.fps_cap", bpo::value(&__gameplay.fps_cap), "@todo: description");
  add_option("gameplay.disable_intro", bpo::bool_switch(&__gameplay.disable_intro), "@todo: description");
  add_option("gameplay.disable_music", bpo::bool_switch(&__gameplay.disable_music), "@todo: description");
  add_option("gameplay.music_volume", bpo::value(&__gameplay.music_volume), "@todo: description");
  add_option("gameplay.compress_saves", bpo::bool_switch(&__gameplay.compress_saves), "@todo: description");

  config_file_options.add(texture_options).add(window_options).add(display_options).add(input_options).add(gameplay_options);

  bool print_help = false;
  bool print_help_full = false;
  bpo::options_description world_options("World Generation Options", 120, 60);
  add_option = world_options.add_options();
  add_option("gen,g", bpo::bool_switch(&__world.generate), "@todo: description");
  add_option("id,i", bpo::value(&__world.id), "@todo: description");
  add_option("seed,s", bpo::value< ::std::uint64_t >()->notifier(::boost::bind(&set_seed, __world, _1)), "@todo: description");
  add_option("param,p", bpo::value< ::std::string >()->notifier(::boost::bind(&set_parameter, __world, _1)), "@todo: description");
  add_option("help,h", bpo::bool_switch(&print_help), "@todo: description");
  add_option("help-full", bpo::bool_switch(&print_help_full), "@todo: description");

  command_line_options.add(world_options).add(texture_options).add(window_options).add(display_options).add(input_options).add(gameplay_options);

  bpo::variables_map variables;
  bpo::store(bpo::parse_command_line(arg_count, arg_values, command_line_options), variables);
  bpo::store(bpo::parse_config_file< char >("dwarf-fortress.conf", config_file_options), variables);
  bpo::notify(variables);

  if (print_help) {
    ::std::cout << world_options;
  } else if (print_help_full) {
    ::std::cout << world_options;
    ::std::cout << texture_options;
    ::std::cout << window_options;
    ::std::cout << display_options;
    ::std::cout << input_options;
    ::std::cout << gameplay_options;
  }
}
