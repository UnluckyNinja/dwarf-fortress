/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/init.hpp"

#include "hidden/global.hpp"
#include "extern/graphics.hpp"
#include "extern/enabler.hpp"

#include "intern/utils/numbers.hpp"
#include "extern/utils/strings.hpp"
#include "intern/config.hpp"

#include <logging/logging.hpp>

#include <fstream>
#include <string>

void initst::begin() {
  static bool called = false;
  if (called) {
    return;
  }
  called = true;

  ::std::ifstream fseed2("data/init/colors.txt");
  if (fseed2.is_open()) {
    ::std::string str;

    while (::std::getline(fseed2, str)) {
      if (str.length() > 1) {
        ::std::string token;
        ::std::string token2;

        grab_token_string_pos(token, str, 1);
        if (str.length() >= token.length() + 2) {
          grab_token_string_pos(token2, str, token.length() + 2);
        }

        if (!token.compare("BLACK_R")) {
          enabler.ccolor[0][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BLACK_G")) {
          enabler.ccolor[0][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BLACK_B")) {
          enabler.ccolor[0][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BLUE_R")) {
          enabler.ccolor[1][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BLUE_G")) {
          enabler.ccolor[1][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BLUE_B")) {
          enabler.ccolor[1][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("GREEN_R")) {
          enabler.ccolor[2][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("GREEN_G")) {
          enabler.ccolor[2][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("GREEN_B")) {
          enabler.ccolor[2][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("CYAN_R")) {
          enabler.ccolor[3][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("CYAN_G")) {
          enabler.ccolor[3][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("CYAN_B")) {
          enabler.ccolor[3][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("RED_R")) {
          enabler.ccolor[4][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("RED_G")) {
          enabler.ccolor[4][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("RED_B")) {
          enabler.ccolor[4][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("MAGENTA_R")) {
          enabler.ccolor[5][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("MAGENTA_G")) {
          enabler.ccolor[5][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("MAGENTA_B")) {
          enabler.ccolor[5][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BROWN_R")) {
          enabler.ccolor[6][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BROWN_G")) {
          enabler.ccolor[6][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("BROWN_B")) {
          enabler.ccolor[6][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGRAY_R")) {
          enabler.ccolor[7][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGRAY_G")) {
          enabler.ccolor[7][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGRAY_B")) {
          enabler.ccolor[7][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("DGRAY_R")) {
          enabler.ccolor[8][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("DGRAY_G")) {
          enabler.ccolor[8][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("DGRAY_B")) {
          enabler.ccolor[8][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LBLUE_R")) {
          enabler.ccolor[9][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LBLUE_G")) {
          enabler.ccolor[9][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LBLUE_B")) {
          enabler.ccolor[9][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGREEN_R")) {
          enabler.ccolor[10][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGREEN_G")) {
          enabler.ccolor[10][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LGREEN_B")) {
          enabler.ccolor[10][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LCYAN_R")) {
          enabler.ccolor[11][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LCYAN_G")) {
          enabler.ccolor[11][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LCYAN_B")) {
          enabler.ccolor[11][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LRED_R")) {
          enabler.ccolor[12][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LRED_G")) {
          enabler.ccolor[12][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LRED_B")) {
          enabler.ccolor[12][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LMAGENTA_R")) {
          enabler.ccolor[13][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LMAGENTA_G")) {
          enabler.ccolor[13][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("LMAGENTA_B")) {
          enabler.ccolor[13][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("YELLOW_R")) {
          enabler.ccolor[14][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("YELLOW_G")) {
          enabler.ccolor[14][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("YELLOW_B")) {
          enabler.ccolor[14][2] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("WHITE_R")) {
          enabler.ccolor[15][0] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("WHITE_G")) {
          enabler.ccolor[15][1] = (float) to_int32(token2) / 255.0f;
        }
        if (!token.compare("WHITE_B")) {
          enabler.ccolor[15][2] = (float) to_int32(token2) / 255.0f;
        }
      }
    }
  }
  fseed2.close();

  config const& conf = config::instance();

  // ------------------------------------------------------------------------------
  // Updating old flag mechanism to avoid breaking unmodifiable code.

  switch (conf.display().mode) {
    case display_config::display_mode::text:
      display.flag.add_flag(INIT_DISPLAY_FLAG_TEXT);
      break;
    case display_config::display_mode::software_2d:
      display.flag.add_flag(INIT_DISPLAY_FLAG_2D);
      break;
    case display_config::display_mode::hardware_2d:
      display.flag.add_flag(INIT_DISPLAY_FLAG_2D);
      display.flag.add_flag(INIT_DISPLAY_FLAG_2DHW);
      break;
    case display_config::display_mode::asynchronous_2d:
      display.flag.add_flag(INIT_DISPLAY_FLAG_2D);
      display.flag.add_flag(INIT_DISPLAY_FLAG_2DHW);
      display.flag.add_flag(INIT_DISPLAY_FLAG_2DASYNC);
      break;
    case display_config::display_mode::accumulation_buffer:
      display.flag.add_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER);
      break;
    case display_config::display_mode::framebuffer:
      display.flag.add_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER);
      break;
    case display_config::display_mode::vertex_buffer_object:
      display.flag.add_flag(INIT_DISPLAY_FLAG_VBO);
      break;
    case display_config::display_mode::shader:
      display.flag.add_flag(INIT_DISPLAY_FLAG_SHADER);
      break;
    case display_config::display_mode::standard:
    default:
      break;
  }

  if (conf.display().use_single_buffer) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER);
  }
  if (conf.texture().use_graphics) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS);
  }
  if (conf.texture().add_black_spaces) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_BLACK_SPACE);
  }
  if (conf.display().use_partial_print) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT);
  }
  if (conf.window().window_not_resizable) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE);
  }
  if (conf.display().use_arb_sync) {
    display.flag.add_flag(INIT_DISPLAY_FLAG_ARB_SYNC);
  }
  display.desired_fullscreen_height = conf.window().fullscreen_height;
  display.desired_fullscreen_width = conf.window().fullscreen_width;
  display.desired_windowed_height = conf.window().window_height;
  display.desired_windowed_width = conf.window().window_width;
  display.partial_print_count = conf.display().partial_print_count;
  display.windowed = conf.window().fullscreen ? InitDisplayWindow::INIT_DISPLAY_WINDOW_FALSE : InitDisplayWindow::INIT_DISPLAY_WINDOW_TRUE;

  if (conf.gameplay().disable_music) {
    media.flag.add_flag(INIT_MEDIA_FLAG_SOUND_OFF);
  }
  if (conf.gameplay().disable_intro) {
    media.flag.add_flag(INIT_MEDIA_FLAG_INTRO_OFF);
  }
  if (conf.gameplay().compress_saves) {
    media.flag.add_flag(INIT_MEDIA_FLAG_COMPRESS_SAVES);
  }
  media.volume = conf.gameplay().music_volume;

  if (conf.input().disable_mouse) {
    input.flag.add_flag(INIT_INPUT_FLAG_MOUSE_OFF);
  }
  if (conf.texture().show_mouse) {
    input.flag.add_flag(INIT_INPUT_FLAG_MOUSE_PICTURE);
  }
  input.hold_time = conf.input().key_hold_delay;
  input.macro_time = conf.input().macro_step_delay;
  input.pause_zoom_no_interface_ms = conf.input().recenter_interface_delay;
  input.repeat_accel_limit = conf.input().key_repeat_acceleration_limit;
  input.repeat_accel_start = conf.input().key_repeat_acceleration_start;
  input.repeat_time = conf.input().key_repeat_delay;
  input.zoom_speed = conf.input().zoom_speed;

  font.use_ttf = conf.texture().use_true_type;

  if (conf.window().keep_topmost) {
    window.flag.add_flag(INIT_WINDOW_FLAG_TOPMOST);
  }
  if (conf.display().use_vsync) {
    window.flag.add_flag(INIT_WINDOW_FLAG_VSYNC_ON);
  } else {
    window.flag.add_flag(INIT_WINDOW_FLAG_VSYNC_OFF);
  }
  if (conf.texture().use_linear_filtering) {
    window.flag.add_flag(INIT_WINDOW_FLAG_TEXTURE_LINEAR);
  }
  // ------------------------------------------------------------------------------

  gps.display_frames = conf.display().show_fps;
  enabler.set_gfps(conf.display().fps_cap);
  enabler.set_fps(conf.gameplay().fps_cap);

  enabler.fullscreen = conf.window().fullscreen;

  enabler.textures.load_multi_pdim(conf.texture().small_font_filename, font.small_font_texpos, 16, 16, true, &font.small_font_dispx, &font.small_font_dispy);
  enabler.textures.load_multi_pdim(conf.texture().large_font_filename, font.large_font_texpos, 16, 16, true, &font.large_font_dispx, &font.large_font_dispy);

  // compute the desired window size, if set to auto
  if (display.desired_windowed_width < MAX_GRID_X && display.desired_windowed_height < MAX_GRID_Y) {
    int dimx = ::std::max(display.desired_windowed_width, 80);
    int dimy = ::std::max(display.desired_windowed_height, 25);
    display.desired_windowed_width = font.small_font_dispx * dimx;
    display.desired_windowed_height = font.small_font_dispy * dimy;
  }
}
