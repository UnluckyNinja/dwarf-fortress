/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */
#define BOOST_PARAMETER_MAX_ARITY 7

#include <GL/glew.h>

#include "sdl_types.hpp"

#include "extern/enabler.hpp"

#include "hidden/dwarf.hpp"
#include "hidden/viewscreen.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"
#include "extern/graphics.hpp"
#include "extern/interface.hpp"

#include "extern/music.hpp"
#include "extern/musicsound.hpp"

#include "intern/enabler/input.hpp"
#include "extern/renderer.hpp"

#include "intern/renderer/2d.hpp"
#include "intern/renderer/accum_buffer.hpp"
#include "intern/renderer/framebuffer.hpp"
#include "intern/renderer/partial.hpp"
#include "intern/renderer/once.hpp"
#include "intern/renderer/vbo.hpp"
#include "intern/renderer/opengl.hpp"

#include "intern/utils/numbers.hpp"
#include "intern/config.hpp"

#include <logging/logging.hpp>

#include <sstream>
#include <map>

enablerst enabler;

// Set to 0 when the game wants to quit
static int loopvar = 1;

// Locates some area of the screen with free space for writing
class gps_locator {
    int y, last_x;
  public:
    gps_locator(int y, int x) {
      this->y = y;
      last_x = x;
    }
    bool is_free(int x) {
      unsigned char c = gps.screen[x * gps.dimy * 4 + y * 4];
      switch (c) {
        case 0:
        case 20:
        case 176:
        case 177:
        case 178:
        case 219:
        case 254:
        case 255:
          return true;
        default:
          return false;
      }
    }
    void operator()(int sz) {
      // First, check if our cached slot will still do
      bool ok = true;
      for (int x = last_x; x < last_x + sz; x++)
        if (!is_free(x)) {
          ok = false;
          break;
        }
      if (ok) {
        // Yeah, okay
        gps.locate(y, last_x);
      } else {
        // Not so okay. Find a new spot.
        int run = 0, x = 0;
        for (; x < gps.dimx; x++) {
          if (is_free(x))
            run++;
          else
            run = 0;
          if (run > sz + 2) { // We pad things a bit for cleanliness.
            ok = true;
            x -= sz + 1;
            break;
          }
        }
        if (ok) {
          // Found a new spot.
          last_x = x;
          gps.locate(y, x);
        } else {
          // Damn it.
          gps.locate(y, last_x);
        }
      }
    }
};

int call_loop(void *dummy) {
  enabler.async_loop();
  return 0;
}

void render_things() {
  //GRAB CURRENT SCREEN AT THE END OF THE LIST
  viewscreenst *currentscreen = &gview.view;
  while (currentscreen->child != NULL)
    currentscreen = currentscreen->child;

  //NO INTERFACE LEFT, LEAVE
  if (currentscreen == &gview.view)
    return;

  if (currentscreen->breakdownlevel == INTERFACE_BREAKDOWN_NONE) {
    currentscreen->render();
  } else
    gps.erasescreen();

  // Render REC when recording macros. Definitely want this screen-specific. Or do we?
  const uint32_t now = SDL_GetTicks();
  if (enabler_input::instance().is_recording() && now % 1000 > 500) {
    gps.locate(0, 20);
    gps.changecolor(4, 1, 1);
    gps.addst("REC");
  }
  // Render PLAY when playing a macro
  if (enabler_input::instance().is_macro_playing() && now % 1000 <= 500) {
    gps.locate(0, 20);
    gps.changecolor(2, 1, 1);
    gps.addst("PLAY");
  }
  if (gps.display_frames) {
    ::std::ostringstream fps_stream;
    fps_stream << "FPS: " << ::std::setw(3) << enabler.calculate_fps() << " (" << enabler.calculate_gfps() << ")";
    ::std::string fps = fps_stream.str();
    gps.changecolor(7, 3, 1);
    static gps_locator fps_locator(0, 25);
    fps_locator(fps.size());
    gps.addst(fps);
  }
}

/*
 ::std::ostream& operator<<(::std::ostream& out, const SDL_keysym& e) {
 out << " scancode: '0x" << (int) e.scancode << "'";
 out << " unicode: '0x" << (int) e.unicode << "'";

 out << " sym: '";
 switch (e.sym) {
 case SDLK_UNKNOWN:
 out << "UNKNOWN";
 break;
 case SDLK_BACKSPACE:
 out << "BACKSPACE";
 break;
 case SDLK_TAB:
 out << "TAB";
 break;
 case SDLK_CLEAR:
 out << "CLEAR";
 break;
 case SDLK_RETURN:
 out << "RETURN";
 break;
 case SDLK_PAUSE:
 out << "PAUSE";
 break;
 case SDLK_ESCAPE:
 out << "ESCAPE";
 break;
 case SDLK_SPACE:
 out << "SPACE";
 break;
 case SDLK_EXCLAIM:
 out << "EXCLAIM";
 break;
 case SDLK_QUOTEDBL:
 out << "QUOTEDBL";
 break;
 case SDLK_HASH:
 out << "HASH";
 break;
 case SDLK_DOLLAR:
 out << "DOLLAR";
 break;
 case SDLK_AMPERSAND:
 out << "AMPERSAND";
 break;
 case SDLK_QUOTE:
 out << "QUOTE";
 break;
 case SDLK_LEFTPAREN:
 out << "LEFTPAREN";
 break;
 case SDLK_RIGHTPAREN:
 out << "RIGHTPAREN";
 break;
 case SDLK_ASTERISK:
 out << "ASTERISK";
 break;
 case SDLK_PLUS:
 out << "PLUS";
 break;
 case SDLK_COMMA:
 out << "COMMA";
 break;
 case SDLK_MINUS:
 out << "MINUS";
 break;
 case SDLK_PERIOD:
 out << "PERIOD";
 break;
 case SDLK_SLASH:
 out << "SLASH";
 break;
 case SDLK_0:
 out << "0";
 break;
 case SDLK_1:
 out << "1";
 break;
 case SDLK_2:
 out << "2";
 break;
 case SDLK_3:
 out << "3";
 break;
 case SDLK_4:
 out << "4";
 break;
 case SDLK_5:
 out << "5";
 break;
 case SDLK_6:
 out << "6";
 break;
 case SDLK_7:
 out << "7";
 break;
 case SDLK_8:
 out << "8";
 break;
 case SDLK_9:
 out << "9";
 break;
 case SDLK_COLON:
 out << "COLON";
 break;
 case SDLK_SEMICOLON:
 out << "SEMICOLON";
 break;
 case SDLK_LESS:
 out << "LESS";
 break;
 case SDLK_EQUALS:
 out << "EQUALS";
 break;
 case SDLK_GREATER:
 out << "GREATER";
 break;
 case SDLK_QUESTION:
 out << "QUESTION";
 break;
 case SDLK_AT:
 out << "AT";
 break;
 case SDLK_LEFTBRACKET:
 out << "LEFTBRACKET";
 break;
 case SDLK_BACKSLASH:
 out << "BACKSLASH";
 break;
 case SDLK_RIGHTBRACKET:
 out << "RIGHTBRACKET";
 break;
 case SDLK_CARET:
 out << "CARET";
 break;
 case SDLK_UNDERSCORE:
 out << "UNDERSCORE";
 break;
 case SDLK_BACKQUOTE:
 out << "BACKQUOTE";
 break;
 case SDLK_a:
 out << "a";
 break;
 case SDLK_b:
 out << "b";
 break;
 case SDLK_c:
 out << "c";
 break;
 case SDLK_d:
 out << "d";
 break;
 case SDLK_e:
 out << "e";
 break;
 case SDLK_f:
 out << "f";
 break;
 case SDLK_g:
 out << "g";
 break;
 case SDLK_h:
 out << "h";
 break;
 case SDLK_i:
 out << "i";
 break;
 case SDLK_j:
 out << "j";
 break;
 case SDLK_k:
 out << "k";
 break;
 case SDLK_l:
 out << "l";
 break;
 case SDLK_m:
 out << "m";
 break;
 case SDLK_n:
 out << "n";
 break;
 case SDLK_o:
 out << "o";
 break;
 case SDLK_p:
 out << "p";
 break;
 case SDLK_q:
 out << "q";
 break;
 case SDLK_r:
 out << "r";
 break;
 case SDLK_s:
 out << "s";
 break;
 case SDLK_t:
 out << "t";
 break;
 case SDLK_u:
 out << "u";
 break;
 case SDLK_v:
 out << "v";
 break;
 case SDLK_w:
 out << "w";
 break;
 case SDLK_x:
 out << "x";
 break;
 case SDLK_y:
 out << "y";
 break;
 case SDLK_z:
 out << "z";
 break;
 case SDLK_DELETE:
 out << "DELETE";
 break;
 case SDLK_WORLD_0:
 out << "WORLD_0";
 break;
 case SDLK_WORLD_1:
 out << "WORLD_1";
 break;
 case SDLK_WORLD_2:
 out << "WORLD_2";
 break;
 case SDLK_WORLD_3:
 out << "WORLD_3";
 break;
 case SDLK_WORLD_4:
 out << "WORLD_4";
 break;
 case SDLK_WORLD_5:
 out << "WORLD_5";
 break;
 case SDLK_WORLD_6:
 out << "WORLD_6";
 break;
 case SDLK_WORLD_7:
 out << "WORLD_7";
 break;
 case SDLK_WORLD_8:
 out << "WORLD_8";
 break;
 case SDLK_WORLD_9:
 out << "WORLD_9";
 break;
 case SDLK_WORLD_10:
 out << "WORLD_10";
 break;
 case SDLK_WORLD_11:
 out << "WORLD_11";
 break;
 case SDLK_WORLD_12:
 out << "WORLD_12";
 break;
 case SDLK_WORLD_13:
 out << "WORLD_13";
 break;
 case SDLK_WORLD_14:
 out << "WORLD_14";
 break;
 case SDLK_WORLD_15:
 out << "WORLD_15";
 break;
 case SDLK_WORLD_16:
 out << "WORLD_16";
 break;
 case SDLK_WORLD_17:
 out << "WORLD_17";
 break;
 case SDLK_WORLD_18:
 out << "WORLD_18";
 break;
 case SDLK_WORLD_19:
 out << "WORLD_19";
 break;
 case SDLK_WORLD_20:
 out << "WORLD_20";
 break;
 case SDLK_WORLD_21:
 out << "WORLD_21";
 break;
 case SDLK_WORLD_22:
 out << "WORLD_22";
 break;
 case SDLK_WORLD_23:
 out << "WORLD_23";
 break;
 case SDLK_WORLD_24:
 out << "WORLD_24";
 break;
 case SDLK_WORLD_25:
 out << "WORLD_25";
 break;
 case SDLK_WORLD_26:
 out << "WORLD_26";
 break;
 case SDLK_WORLD_27:
 out << "WORLD_27";
 break;
 case SDLK_WORLD_28:
 out << "WORLD_28";
 break;
 case SDLK_WORLD_29:
 out << "WORLD_29";
 break;
 case SDLK_WORLD_30:
 out << "WORLD_30";
 break;
 case SDLK_WORLD_31:
 out << "WORLD_31";
 break;
 case SDLK_WORLD_32:
 out << "WORLD_32";
 break;
 case SDLK_WORLD_33:
 out << "WORLD_33";
 break;
 case SDLK_WORLD_34:
 out << "WORLD_34";
 break;
 case SDLK_WORLD_35:
 out << "WORLD_35";
 break;
 case SDLK_WORLD_36:
 out << "WORLD_36";
 break;
 case SDLK_WORLD_37:
 out << "WORLD_37";
 break;
 case SDLK_WORLD_38:
 out << "WORLD_38";
 break;
 case SDLK_WORLD_39:
 out << "WORLD_39";
 break;
 case SDLK_WORLD_40:
 out << "WORLD_40";
 break;
 case SDLK_WORLD_41:
 out << "WORLD_41";
 break;
 case SDLK_WORLD_42:
 out << "WORLD_42";
 break;
 case SDLK_WORLD_43:
 out << "WORLD_43";
 break;
 case SDLK_WORLD_44:
 out << "WORLD_44";
 break;
 case SDLK_WORLD_45:
 out << "WORLD_45";
 break;
 case SDLK_WORLD_46:
 out << "WORLD_46";
 break;
 case SDLK_WORLD_47:
 out << "WORLD_47";
 break;
 case SDLK_WORLD_48:
 out << "WORLD_48";
 break;
 case SDLK_WORLD_49:
 out << "WORLD_49";
 break;
 case SDLK_WORLD_50:
 out << "WORLD_50";
 break;
 case SDLK_WORLD_51:
 out << "WORLD_51";
 break;
 case SDLK_WORLD_52:
 out << "WORLD_52";
 break;
 case SDLK_WORLD_53:
 out << "WORLD_53";
 break;
 case SDLK_WORLD_54:
 out << "WORLD_54";
 break;
 case SDLK_WORLD_55:
 out << "WORLD_55";
 break;
 case SDLK_WORLD_56:
 out << "WORLD_56";
 break;
 case SDLK_WORLD_57:
 out << "WORLD_57";
 break;
 case SDLK_WORLD_58:
 out << "WORLD_58";
 break;
 case SDLK_WORLD_59:
 out << "WORLD_59";
 break;
 case SDLK_WORLD_60:
 out << "WORLD_60";
 break;
 case SDLK_WORLD_61:
 out << "WORLD_61";
 break;
 case SDLK_WORLD_62:
 out << "WORLD_62";
 break;
 case SDLK_WORLD_63:
 out << "WORLD_63";
 break;
 case SDLK_WORLD_64:
 out << "WORLD_64";
 break;
 case SDLK_WORLD_65:
 out << "WORLD_65";
 break;
 case SDLK_WORLD_66:
 out << "WORLD_66";
 break;
 case SDLK_WORLD_67:
 out << "WORLD_67";
 break;
 case SDLK_WORLD_68:
 out << "WORLD_68";
 break;
 case SDLK_WORLD_69:
 out << "WORLD_69";
 break;
 case SDLK_WORLD_70:
 out << "WORLD_70";
 break;
 case SDLK_WORLD_71:
 out << "WORLD_71";
 break;
 case SDLK_WORLD_72:
 out << "WORLD_72";
 break;
 case SDLK_WORLD_73:
 out << "WORLD_73";
 break;
 case SDLK_WORLD_74:
 out << "WORLD_74";
 break;
 case SDLK_WORLD_75:
 out << "WORLD_75";
 break;
 case SDLK_WORLD_76:
 out << "WORLD_76";
 break;
 case SDLK_WORLD_77:
 out << "WORLD_77";
 break;
 case SDLK_WORLD_78:
 out << "WORLD_78";
 break;
 case SDLK_WORLD_79:
 out << "WORLD_79";
 break;
 case SDLK_WORLD_80:
 out << "WORLD_80";
 break;
 case SDLK_WORLD_81:
 out << "WORLD_81";
 break;
 case SDLK_WORLD_82:
 out << "WORLD_82";
 break;
 case SDLK_WORLD_83:
 out << "WORLD_83";
 break;
 case SDLK_WORLD_84:
 out << "WORLD_84";
 break;
 case SDLK_WORLD_85:
 out << "WORLD_85";
 break;
 case SDLK_WORLD_86:
 out << "WORLD_86";
 break;
 case SDLK_WORLD_87:
 out << "WORLD_87";
 break;
 case SDLK_WORLD_88:
 out << "WORLD_88";
 break;
 case SDLK_WORLD_89:
 out << "WORLD_89";
 break;
 case SDLK_WORLD_90:
 out << "WORLD_90";
 break;
 case SDLK_WORLD_91:
 out << "WORLD_91";
 break;
 case SDLK_WORLD_92:
 out << "WORLD_92";
 break;
 case SDLK_WORLD_93:
 out << "WORLD_93";
 break;
 case SDLK_WORLD_94:
 out << "WORLD_94";
 break;
 case SDLK_WORLD_95:
 out << "WORLD_95";
 break;
 case SDLK_KP0:
 out << "KP0";
 break;
 case SDLK_KP1:
 out << "KP1";
 break;
 case SDLK_KP2:
 out << "KP2";
 break;
 case SDLK_KP3:
 out << "KP3";
 break;
 case SDLK_KP4:
 out << "KP4";
 break;
 case SDLK_KP5:
 out << "KP5";
 break;
 case SDLK_KP6:
 out << "KP6";
 break;
 case SDLK_KP7:
 out << "KP7";
 break;
 case SDLK_KP8:
 out << "KP8";
 break;
 case SDLK_KP9:
 out << "KP9";
 break;
 case SDLK_KP_PERIOD:
 out << "KP_PERIOD";
 break;
 case SDLK_KP_DIVIDE:
 out << "KP_DIVIDE";
 break;
 case SDLK_KP_MULTIPLY:
 out << "KP_MULTIPLY";
 break;
 case SDLK_KP_MINUS:
 out << "KP_MINUS";
 break;
 case SDLK_KP_PLUS:
 out << "KP_PLUS";
 break;
 case SDLK_KP_ENTER:
 out << "KP_ENTER";
 break;
 case SDLK_KP_EQUALS:
 out << "KP_EQUALS";
 break;
 case SDLK_UP:
 out << "UP";
 break;
 case SDLK_DOWN:
 out << "DOWN";
 break;
 case SDLK_RIGHT:
 out << "RIGHT";
 break;
 case SDLK_LEFT:
 out << "LEFT";
 break;
 case SDLK_INSERT:
 out << "INSERT";
 break;
 case SDLK_HOME:
 out << "HOME";
 break;
 case SDLK_END:
 out << "END";
 break;
 case SDLK_PAGEUP:
 out << "PAGEUP";
 break;
 case SDLK_PAGEDOWN:
 out << "PAGEDOWN";
 break;
 case SDLK_F1:
 out << "F1";
 break;
 case SDLK_F2:
 out << "F2";
 break;
 case SDLK_F3:
 out << "F3";
 break;
 case SDLK_F4:
 out << "F4";
 break;
 case SDLK_F5:
 out << "F5";
 break;
 case SDLK_F6:
 out << "F6";
 break;
 case SDLK_F7:
 out << "F7";
 break;
 case SDLK_F8:
 out << "F8";
 break;
 case SDLK_F9:
 out << "F9";
 break;
 case SDLK_F10:
 out << "F10";
 break;
 case SDLK_F11:
 out << "F11";
 break;
 case SDLK_F12:
 out << "F12";
 break;
 case SDLK_F13:
 out << "F13";
 break;
 case SDLK_F14:
 out << "F14";
 break;
 case SDLK_F15:
 out << "F15";
 break;
 case SDLK_NUMLOCK:
 out << "NUMLOCK";
 break;
 case SDLK_CAPSLOCK:
 out << "CAPSLOCK";
 break;
 case SDLK_SCROLLOCK:
 out << "SCROLLOCK";
 break;
 case SDLK_RSHIFT:
 out << "RSHIFT";
 break;
 case SDLK_LSHIFT:
 out << "LSHIFT";
 break;
 case SDLK_RCTRL:
 out << "RCTRL";
 break;
 case SDLK_LCTRL:
 out << "LCTRL";
 break;
 case SDLK_RALT:
 out << "RALT";
 break;
 case SDLK_LALT:
 out << "LALT";
 break;
 case SDLK_RMETA:
 out << "RMETA";
 break;
 case SDLK_LMETA:
 out << "LMETA";
 break;
 case SDLK_LSUPER:
 out << "LSUPER";
 break;
 case SDLK_RSUPER:
 out << "RSUPER";
 break;
 case SDLK_MODE:
 out << "MODE";
 break;
 case SDLK_COMPOSE:
 out << "COMPOSE";
 break;
 case SDLK_HELP:
 out << "HELP";
 break;
 case SDLK_PRINT:
 out << "PRINT";
 break;
 case SDLK_SYSREQ:
 out << "SYSREQ";
 break;
 case SDLK_BREAK:
 out << "BREAK";
 break;
 case SDLK_MENU:
 out << "MENU";
 break;
 case SDLK_POWER:
 out << "POWER";
 break;
 case SDLK_EURO:
 out << "EURO";
 break;
 case SDLK_UNDO:
 out << "UNDO";
 break;
 case SDLK_LAST:
 out << "LAST";
 break;
 }
 out << "'";

 out << " mods: ' ";
 if (e.mod == KMOD_NONE) {
 out << "none ";
 }
 if (e.mod & KMOD_LSHIFT) {
 out << "lshift ";
 }
 if (e.mod & KMOD_RSHIFT) {
 out << "rshift ";
 }
 if (e.mod & KMOD_LCTRL) {
 out << "lctrl ";
 }
 if (e.mod & KMOD_RCTRL) {
 out << "rctrl ";
 }
 if (e.mod & KMOD_LALT) {
 out << "lalt ";
 }
 if (e.mod & KMOD_RALT) {
 out << "ralt ";
 }
 if (e.mod & KMOD_LMETA) {
 out << "lmeta ";
 }
 if (e.mod & KMOD_RMETA) {
 out << "rmeta ";
 }
 if (e.mod & KMOD_NUM) {
 out << "num ";
 }
 if (e.mod & KMOD_CAPS) {
 out << "caps ";
 }
 if (e.mod & KMOD_MODE) {
 out << "mode ";
 }
 if (e.mod & KMOD_RESERVED) {
 out << "reserved ";
 }
 out << "'";

 return out;
 }

 ::std::ostream& operator<<(::std::ostream& out, const sdl_keyboard_event_t& e) {
 switch (e.state) {
 case SDL_PRESSED:
 out << " state: pressed.";
 break;
 case SDL_RELEASED:
 out << " state: released.";
 break;
 }
 out << " which: '0x" << (int) e.which << "'";
 out << e.keysym;

 return out;
 }

 ::std::ostream& operator<<(::std::ostream& out, const sdl_event_t& e) {
 SDL_EventType type = SDL_EventType(e.type);

 out << ::std::hex;
 out << "event";
 switch (type) {
 case SDL_NOEVENT:
 out << " no.";
 break;
 case SDL_ACTIVEEVENT:
 out << " active.";
 out << " state: '0x" << (int) e.active.state << "'";
 out << " gain: '0x" << (int) e.active.gain << "'";
 break;
 case SDL_KEYDOWN:
 out << " key_down.";
 out << e.key;
 break;
 case SDL_KEYUP:
 out << " key_up.";
 out << e.key;
 break;
 case SDL_MOUSEMOTION:
 out << " mouse_motion.";
 out << " which: '0x" << (int) e.motion.which << "'";
 switch (e.motion.state) {
 case SDL_PRESSED:
 out << " state: pressed.";
 break;
 case SDL_RELEASED:
 out << " state: released.";
 break;
 }
 out << " x: '0x" << (int) e.motion.x << "'";
 out << " xrel: '0x" << (int) e.motion.xrel << "'";
 out << " y: '0x" << (int) e.motion.y << "'";
 out << " yrel: '0x" << (int) e.motion.yrel << "'";
 break;
 case SDL_MOUSEBUTTONDOWN:
 out << " mouse_button_down.";
 out << " which: '0x" << (int) e.button.which << "'";
 switch (e.button.state) {
 case SDL_PRESSED:
 out << " state: pressed.";
 break;
 case SDL_RELEASED:
 out << " state: released.";
 break;
 }
 out << " button: '0x" << (int) e.button.button << "'";
 out << " x: '0x" << (int) e.button.x << "'";
 out << " y: '0x" << (int) e.button.y << "'";
 break;
 case SDL_MOUSEBUTTONUP:
 out << " mouse_button_up.";
 out << " which: '0x" << (int) e.button.which << "'";
 switch (e.button.state) {
 case SDL_PRESSED:
 out << " state: pressed.";
 break;
 case SDL_RELEASED:
 out << " state: released.";
 break;
 }
 out << " button: '0x" << (int) e.button.button << "'";
 out << " x: '0x" << (int) e.button.x << "'";
 out << " y: '0x" << (int) e.button.y << "'";
 break;
 case SDL_JOYAXISMOTION:
 out << " joy_axis_motion.";
 break;
 case SDL_JOYBALLMOTION:
 out << " joy_ball_motion.";
 break;
 case SDL_JOYHATMOTION:
 out << " joy_hat_motion.";
 break;
 case SDL_JOYBUTTONDOWN:
 out << " joy_button_down.";
 break;
 case SDL_JOYBUTTONUP:
 out << " joy_button_up.";
 break;
 case SDL_QUIT:
 out << " quit.";
 break;
 case SDL_SYSWMEVENT:
 out << " sys_wm_event.";
 break;
 case SDL_EVENT_RESERVEDA:
 out << " reserveda.";
 break;
 case SDL_EVENT_RESERVEDB:
 out << " reservedb.";
 break;
 case SDL_VIDEORESIZE:
 out << " video_resize.";
 out << " w: '0x" << (int) e.resize.w << "'";
 out << " h: '0x" << (int) e.resize.h << "'";
 break;
 case SDL_VIDEOEXPOSE:
 out << " video_expose.";
 break;
 case SDL_EVENT_RESERVED2:
 out << " reserved2.";
 break;
 case SDL_EVENT_RESERVED3:
 out << " reserved3.";
 break;
 case SDL_EVENT_RESERVED4:
 out << " reserved4.";
 break;
 case SDL_EVENT_RESERVED5:
 out << " reserved5.";
 break;
 case SDL_EVENT_RESERVED6:
 out << " reserved6.";
 break;
 case SDL_EVENT_RESERVED7:
 out << " reserved7.";
 break;
 case SDL_USEREVENT:
 out << " user_events.";
 out << " code: '0x" << (int) e.user.code << "'";
 out << " data1: '0x" << e.user.data1 << "'";
 out << " data2: '0x" << e.user.data2 << "'";
 break;
 case SDL_NUMEVENTS:
 out << " num_events.";
 break;
 }

 out << ::std::dec;

 return out;
 }
 */

void enablerst::eventLoop_SDL() {
  config const& conf = config::instance();

  sdl_event_t event;
  const SDL_Surface *screen = SDL_GetVideoSurface();
  uint32_t mouse_lastused = 0;
  SDL_ShowCursor(SDL_DISABLE);

  // Initialize the grid
  renderer->resize(screen->w, screen->h);

  while (loopvar) {
    uint32_t now = SDL_GetTicks();
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

      //      __infoM(sdl)
      //        << event;

      // Handle SDL events
      switch (event.type) {
        case SDL_NOEVENT:
        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        case SDL_SYSWMEVENT:
        case SDL_EVENT_RESERVEDA:
        case SDL_EVENT_RESERVEDB:
        case SDL_EVENT_RESERVED2:
        case SDL_EVENT_RESERVED3:
        case SDL_EVENT_RESERVED4:
        case SDL_EVENT_RESERVED5:
        case SDL_EVENT_RESERVED6:
        case SDL_EVENT_RESERVED7:
        case SDL_USEREVENT:
        case SDL_NUMEVENTS:
          break;
        case SDL_KEYDOWN:
          // Disable mouse if it's been long enough
          if (mouse_lastused + 5000 < now) {
            if (conf.texture().show_mouse) {
              // hide the mouse picture
              // enabler.set_tile(0, TEXTURE_MOUSE, enabler.mouse_x, enabler.mouse_y);
            }
            SDL_ShowCursor(SDL_DISABLE);
          }
        case SDL_KEYUP:
        case SDL_QUIT:
          enabler_input::instance().add_input(event, now);
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          if (!conf.input().disable_mouse) {
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
              enabler_input::instance().add_input(event, now);
          }
          break;
        case SDL_MOUSEMOTION:
          // Deal with the mouse hiding bit
          mouse_lastused = now;
          if (conf.texture().show_mouse) {
            // turn on mouse picture
            // enabler.set_tile(gps.tex_pos[TEXTURE_MOUSE], TEXTURE_MOUSE,enabler.mouse_x, enabler.mouse_y);
          } else {
            SDL_ShowCursor(SDL_ENABLE);
          }
          break;
        case SDL_ACTIVEEVENT:
          enabler_input::instance().clear_input();
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
    if (!conf.input().disable_mouse) {
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

#ifdef CURSES
void enablerst::eventLoop_ncurses() {
  int x, y, oldx = 0, oldy = 0;
  renderer_curses *renderer = static_cast< renderer_curses* > (this->renderer);

  while (loopvar) {
    // Check for terminal resize
    getmaxyx(stdscr, y, x);
    if (y != oldy || x != oldx) {
      pause_async_loop();
      renderer->resize(x, y);
      unpause_async_loop();
      oldx = x;
      oldy = y;
    }

    // Deal with input
    uint32_t now = SDL_GetTicks();
    // Read keyboard input, if any, and transform to artificial SDL
    // events for enabler_input.
    int key;
    bool paused_loop = false;
    while ((key = getch_utf8())) {
      if (!paused_loop) {
        pause_async_loop();
        paused_loop = true;
      }
      bool esc = false;
      if (key == KEY_MOUSE) {
        MEVENT ev;
        if (getmouse(&ev) == OK) {
          // TODO: Deal with curses mouse input. And turn it on above.
        }
      } else if (key == -27) { // esc
        int second = getch_utf8();
        if (second) { // That was an escape sequence
          esc = true;
          key = second;
        }
      }
      enabler_input::instance().add_input_ncurses(key, now, esc);
    }

    if (paused_loop)
    unpause_async_loop();

    // Run the common logic
    do_frame();
  }
}
#endif

void enablerst::update_gfps() {
  do_update_fps(gframe_timings, gframe_sum, gframe_last, calculated_gfps);
}

void enablerst::pause_async_loop() {
  struct async_cmd cmd;
  cmd.cmd = async_cmd::pause;
  async_tobox.write(cmd);
  async_wait();
}

// Wait until the previous command has been acknowledged, /or/
// async_loop has quit. Incidentally execute any requests in the
// meantime.
void enablerst::async_wait() {
  if (loopvar == 0)
    return;
  async_msg r;
  bool reset_textures = false;
  for (;;) {
    async_frombox.read(r);
    switch (r.msg) {
      case async_msg::quit:
        loopvar = 0;
        return;
      case async_msg::complete:
        if (reset_textures) {
          __info
            << "Resetting textures";
          textures.remove_uploaded_textures();
          textures.upload_textures();
        }
        return;
      case async_msg::set_fps:
        set_fps(r.fps);
        async_fromcomplete.write();
        break;
      case async_msg::set_gfps:
        set_gfps(r.fps);
        async_fromcomplete.write();
        break;
      case async_msg::push_resize:
        override_grid_size(r.x, r.y);
        async_fromcomplete.write();
        break;
      case async_msg::pop_resize:
        release_grid_size();
        async_fromcomplete.write();
        break;
      case async_msg::reset_textures:
        reset_textures = true;
        break;
      default:
        __info
          << "EMERGENCY: Unknown case in async_wait";
        abort();
    }
  }
}

void enablerst::unpause_async_loop() {
  struct async_cmd cmd;
  cmd.cmd = async_cmd::start;
  async_tobox.write(cmd);
}

enablerst::enablerst() {
  fullscreen = false;
  sync = NULL;
  renderer = NULL;
  calculated_fps = calculated_gfps = frame_sum = gframe_sum = frame_last = gframe_last = 0;
  fps = 100;
  gfps = 20;
  fps_per_gfps = fps / gfps;
  last_tick = 0;
}

int enablerst::loop(::std::string cmdline) {
  display_config const& display = config::instance().display();

  command_line = cmdline;

  // Initialize the tick counters
  set_simticks(0);
  set_gputicks(0);

  // Call DF's initialization routine
  if (!beginroutine()) {
    exit(EXIT_FAILURE);
  }

  switch (display.mode) {
#ifdef CURSES
    case display_config::display_mode::text:
    renderer = new renderer_curses();
    break;
#else
    case display_config::display_mode::text:
      __error
        << "text renderer not supported, falling back to 2d renderer.";
      renderer = new renderer_2d();
      break;
#endif
    case display_config::display_mode::software_2d:
    case display_config::display_mode::hardware_2d:
    case display_config::display_mode::asynchronous_2d:
      renderer = new renderer_2d();
      break;
    case display_config::display_mode::accumulation_buffer:
      renderer = new renderer_accum_buffer();
      break;
    case display_config::display_mode::framebuffer:
      renderer = new renderer_framebuffer();
      break;
    case display_config::display_mode::vertex_buffer_object:
      renderer = new renderer_vbo();
      break;
    case display_config::display_mode::shader:
      renderer = new renderer_opengl();
      break;
    case display_config::display_mode::standard:
    default:
      if (display.use_partial_print) {
        if (display.partial_print_count > 0) {
          renderer = new renderer_partial();
        } else {
          renderer = new renderer_once();
        }
      } else {
        renderer = new renderer_opengl();
      }
  }

  switch (display.mode) {
#ifdef CURSES
    case display_config::display_mode::text:
    eventLoop_ncurses();
    break;
#else
    case display_config::display_mode::text:
#endif
    case display_config::display_mode::software_2d:
    case display_config::display_mode::hardware_2d:
    case display_config::display_mode::asynchronous_2d:
    case display_config::display_mode::accumulation_buffer:
    case display_config::display_mode::framebuffer:
    case display_config::display_mode::vertex_buffer_object:
    case display_config::display_mode::shader:
    case display_config::display_mode::standard:
    default:
      SDL_EnableUNICODE(1);
      eventLoop_SDL();
      break;
  }

  //  // Allocate a renderer
  //  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_TEXT)) {
  //#ifdef CURSES
  //    renderer = new renderer_curses();
  //#else
  //    __error
  //      << "TEXT not supported on windows";
  //    exit(EXIT_FAILURE);
  //#endif
  //  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_2D)) {
  //    renderer = new renderer_2d();
  //  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_ACCUM_BUFFER)) {
  //    renderer = new renderer_accum_buffer();
  //  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_FRAME_BUFFER)) {
  //    renderer = new renderer_framebuffer();
  //  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_PARTIAL_PRINT)) {
  //    //    if (init.display.partial_print_count)
  //    //      renderer = new renderer_partial();
  //    //    else
  //    //      renderer = new renderer_once();
  //    if (config::instance().display().use_partial_print) {
  //      renderer = new renderer_partial();
  //    } else {
  //      renderer = new renderer_once();
  //    }
  //  } else if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_VBO)) {
  //    renderer = new renderer_vbo();
  //  } else {
  //    renderer = new renderer_opengl();
  //  }

  endroutine();

  // Clean up graphical resources
  delete renderer;

  return 0;
}

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
            // __info << "Paused";
            async_frombox.write(async_msg(async_msg::complete));
            break;
          case async_cmd::start:
            async_paused = false;
            async_frames = 0;
            // __info << "UNpaused";
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
            if (async_frames > (unsigned int) fps * 3)
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
      inc_simticks();
      async_frames--;
      if (async_frames < 0)
        async_frames = 0;
      update_fps();
    }
    SDL_NumJoysticks(); // Hook for dfhack
  }
}

void enablerst::do_frame() {
  // Check how long it's been, exactly
  const uint32_t now = SDL_GetTicks();
  const uint32_t interval = ::numbers::clamp(now - last_tick, 0u, 1000u); // Anything above a second doesn't count
  // cout << last_tick << " + " << interval << " = " << now << endl;
  last_tick = now;

  // Update outstanding-frame counts
  outstanding_frames += interval * fps / 1000;
  outstanding_gframes += interval * gfps / 1000;
  if (outstanding_gframes > 3) {
    outstanding_gframes = 3;
  }
  // cout << outstanding_frames << " " << outstanding_gframes << endl;

  // Update the loop's tick-counter suitably
  if (outstanding_frames >= 1) {
    async_cmd cmd(async_cmd::inc);
    cmd.val = outstanding_frames;
    outstanding_frames -= cmd.val;
    async_tobox.write(cmd);
  }

  // Store the current time, for things that are fine with approximations
  enabler.clock = SDL_GetTicks();

  // If it's time to render..
  if (outstanding_gframes >= 1 && (!sync || glClientWaitSync(sync, 0, 0) == GL_ALREADY_SIGNALED)) {
    // Get the async-loop to render_things
    async_cmd cmd(async_cmd::render);
    async_tobox.write(cmd);
    async_wait();
    // Then finish here
    renderer->display();
    renderer->render();

    inc_gputicks();
    outstanding_gframes--;
  }

  // Sleep until the next gframe
  if (outstanding_gframes < 1) {
    float fragment = 1 - outstanding_gframes;
    float milliseconds = fragment / gfps * 1000;
    // cout << milliseconds << endl;
    SDL_Delay(milliseconds);
  }
}

void enablerst::set_fps(int fps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_fps);
    m.fps = fps;
    async_paused = true;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (fps == 0)
      fps = 1048576;
    this->fps = fps;
    fps_per_gfps = fps / gfps;
    struct async_cmd cmd;
    cmd.cmd = async_cmd::set_fps;
    cmd.val = fps;
    async_tobox.write(cmd);
    async_tobox.write(async_cmd(async_cmd::start));
  }
}

void enablerst::set_gfps(int gfps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_gfps);
    m.fps = gfps;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (gfps == 0)
      gfps = 50;
    this->gfps = gfps;
    fps_per_gfps = fps / gfps;
  }
}

int enablerst::get_fps() {
  return (int) fps;
}

int enablerst::get_gfps() {
  return (int) gfps;
}

int enablerst::calculate_fps() {
  if (frame_timings.size() < 50)
    return get_fps();
  else
    return calculated_fps;
}

int enablerst::calculate_gfps() {
  if (gframe_timings.size() < 50)
    return get_gfps();
  else
    return calculated_gfps;
}

void enablerst::do_update_fps(::std::queue< int > &q, int &sum, int &last, int &calc) {
  while (q.size() > 50 && sum > 10000) {
    sum -= q.front();
    q.pop();
  }
  const int now = SDL_GetTicks();
  const int interval = now - last;
  q.push(interval);
  sum += interval;
  last = now;
  if (sum)
    calc = q.size() * 1000 / sum;
}

void enablerst::clear_fps() {
  while (frame_timings.size())
    frame_timings.pop();
  frame_sum = 0;
  frame_last = SDL_GetTicks();
  calculated_fps = get_fps();
}

void enablerst::update_fps() {
  do_update_fps(frame_timings, frame_sum, frame_last, calculated_fps);
}

void enablerst::reset_textures() {
  async_frombox.write(async_msg(async_msg::reset_textures));
}

bool enablerst::uses_opengl() {
  if (!renderer)
    return false;
  return renderer->uses_opengl();
}

void enablerst::override_grid_size(int x, int y) {
  if (SDL_ThreadID() != renderer_threadid) {
    // Ask the renderer to do it
    async_msg m(async_msg::push_resize);
    m.x = x;
    m.y = y;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    // We are the renderer; do it.
    overridden_grid_sizes.push(::std::make_pair(init.display.grid_x, init.display.grid_y));
    renderer->grid_resize(x, y);
  }
}

void enablerst::release_grid_size() {
  if (SDL_ThreadID() != renderer_threadid) {
    async_frombox.write(async_msg(async_msg::pop_resize));
    async_fromcomplete.read();
  } else {
    if (!overridden_grid_sizes.size())
      return;
    // FIXME: Find out whatever is causing release to be called too rarely; right now
    // we're overriding once per movie but apparently only releasing for the last one.
    ::std::pair< int, int > sz;
    while (overridden_grid_sizes.size()) {
      sz = overridden_grid_sizes.top();
      overridden_grid_sizes.pop();
    }
    zoom_display(zoom_resetgrid);
  }
}

void enablerst::zoom_display(zoom_commands command) {
  async_zoom.write(command);
}

bool enablerst::is_fullscreen() {
  return fullscreen;
}

void enablerst::toggle_fullscreen() {
  fullscreen = !fullscreen;
  async_zoom.write(zoom_fullscreen);
}

void enablerst::set_simticks(int simticks_in) {
  //      ::boost::lock_guard< ::boost::mutex > lock(simticks_mutex);
  simticks.write(simticks_in);
}

int enablerst::inc_simticks() {
  //      ::boost::lock_guard< ::boost::mutex > lock(simticks_mutex);
  int simticks_out = simticks.read();
  simticks.write(++simticks_out);
  return simticks_out;
}

int enablerst::get_simticks() {
  //      ::boost::lock_guard< ::boost::mutex > lock(simticks_mutex);
  return simticks.read();
}

void enablerst::set_gputicks(int gputicks_in) {
  //      ::boost::lock_guard< ::boost::mutex > lock(gputicks_mutex);
  gputicks.write(gputicks_in);
}

int enablerst::inc_gputicks() {
  //      ::boost::lock_guard< ::boost::mutex > lock(gputicks_mutex);
  int gputicks_out = gputicks.read();
  gputicks.write(++gputicks_out);
  return gputicks_out;
}

int enablerst::get_gputicks() {
  //      ::boost::lock_guard< ::boost::mutex > lock(gputicks_mutex);
  return gputicks.read();
}
