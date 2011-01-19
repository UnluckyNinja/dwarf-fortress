/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */
#define BOOST_PARAMETER_MAX_ARITY 7

#include "extern/interface.hpp"

#include "hidden/dwarf.hpp"
#include "hidden/global.hpp"

#include "intern/enabler/input.hpp"

#include "intern/screen/macro_save.hpp"
#include "intern/screen/macro_load.hpp"

#include "extern/enabler.hpp"

char interfacest::loop() {
  //NO INTERFACE LEFT, QUIT
  if (view.child == 0)
    return 1;

  //GRAB CURRENT SCREEN AT THE END OF THE LIST
  viewscreenst *currentscreen = grab_lastscreen();
  //MOVE SCREENS BACK
  switch (currentscreen->breakdownlevel) {
    case INTERFACE_BREAKDOWN_NONE: {

      currentscreen->logic();

      if (currentscreen->movies_okay()) {
        //HANDLE MOVIES
        handlemovie(0);
      }

      // Process as much input as possible. Some screens can't handle multiple input events
      // per logic call (retain_nonzero_input, and any alteration to the window setup
      // requires us to stop until the next logic call.
      for (;;) {
        if (currentscreen->child || currentscreen->breakdownlevel != INTERFACE_BREAKDOWN_NONE)
          break; // Some previous input or logic had the effect of switching screens

        if (flag & INTERFACEFLAG_RETAIN_NONZERO_INPUT) {
          flag &= ~INTERFACEFLAG_RETAIN_NONZERO_INPUT;
          break;
        } else {
          ::std::set< interface_key_t > era = enabler_input::instance().get_input();
          if (era.size() == 0) {
            if (enabler.mouse_lbut || enabler.mouse_rbut)
              currentscreen->feed(era);
            break;
          }

          if (era.count(INTERFACEKEY_OPTIONS) && !currentscreen->key_conflict(INTERFACEKEY_OPTIONS)) {
            //PEEL BACK ALL SCREENS TO THE CURRENT OPTION SCREEN IF THERE IS ONE
            //UNLESS THERE IS A BLOCKING SCREEN LIKE THE REGION MAKER
            viewscreenst *opscreen = &view;
            while (opscreen != NULL) {
              if (opscreen->is_option_screen()) {
                opscreen->option_key_pressed = 1;
                while (opscreen->child != NULL) {
                  if (opscreen->child->is_option_screen() == 2) {
                    opscreen->child->option_key_pressed = 1;
                    opscreen->option_key_pressed = 0;
                    break;
                  }
                  removescreen(opscreen->child);
                }
                break;
              }
              opscreen = opscreen->child;
            }
            //NEED A NEW OPTIONS SCREEN?
            if (opscreen == NULL)
              dwarf_option_screen();

            era.clear();
            continue;
          }
          //DO MOVIE COMMANDS
          if (era.count(INTERFACEKEY_MOVIES) && !currentscreen->key_conflict(INTERFACEKEY_MOVIES))
            if (currentscreen->movies_okay())
              use_movie_input();
          if (era.count(INTERFACEKEY_HELP) && !currentscreen->key_conflict(INTERFACEKEY_HELP))
            currentscreen->help();
          // Zoom commands
          if (era.count(INTERFACEKEY_ZOOM_IN))
            enabler.zoom_display(zoom_in);
          if (era.count(INTERFACEKEY_ZOOM_OUT))
            enabler.zoom_display(zoom_out);
          if (era.count(INTERFACEKEY_ZOOM_RESET))
            enabler.zoom_display(zoom_reset);
          // Macro commands
          if (era.count(INTERFACEKEY_RECORD_MACRO)) {
            if (enabler_input::instance().is_recording())
              enabler_input::instance().record_stop();
            else
              enabler_input::instance().record_input();
          }
          if (era.count(INTERFACEKEY_PLAY_MACRO))
            enabler_input::instance().play_macro();
          if (era.count(INTERFACEKEY_SAVE_MACRO))
            gview.addscreen(new MacroScreenSave(), INTERFACE_PUSH_AT_BACK, NULL);
          if (era.count(INTERFACEKEY_LOAD_MACRO))
            gview.addscreen(new MacroScreenLoad(), INTERFACE_PUSH_AT_BACK, NULL);
          // Feed input
          currentscreen->feed(era);
          if (era.count(INTERFACEKEY_TOGGLE_FULLSCREEN)) {
            enabler.toggle_fullscreen();
          }
          if (era.count(INTERFACEKEY_FPS_UP)) {
            int fps = enabler.get_fps();
            enabler.set_fps(fps + fps / 10);
            enabler.clear_fps();
          }
          if (era.count(INTERFACEKEY_FPS_DOWN)) {
            int fps = enabler.get_fps();
            enabler.set_fps(fps - fps / 10);
            enabler.clear_fps();
          }
        }
      }
      break;
    } // case INTERFACE_BREAKDOWN_NONE

    case INTERFACE_BREAKDOWN_QUIT: {
      handlemovie(1);
      return 1;
    }
    case INTERFACE_BREAKDOWN_STOPSCREEN:
      if (currentscreen->movies_okay()) {
        //HANDLE MOVIES
        handlemovie(0);
      }

      removescreen(currentscreen);
      break;
    case INTERFACE_BREAKDOWN_TOFIRST:
      if (currentscreen->movies_okay()) {
        //HANDLE MOVIES
        handlemovie(0);
      }

      remove_to_first();
      break;
  }

  return 0;
}

void interfacest::addscreen(viewscreenst *scr, char pushtype, viewscreenst *relate) {
  gps.force_full_display_count += 2;

  switch (pushtype) {
    case INTERFACE_PUSH_AS_PARENT:
      insertscreen_as_parent(scr, relate);
      break;
    case INTERFACE_PUSH_AS_CHILD:
      insertscreen_as_child(scr, relate);
      break;
    case INTERFACE_PUSH_AT_FRONT:
      insertscreen_at_front(scr);
      break;
    default:
      insertscreen_at_back(scr);
      break;
  }

  //WHENEVER A SCREEN IS ADDED, END ANNOUNCEMENTS
  if (gamemode == GAMEMODE_DWARF)
    dwarf_end_announcements();
}

void interfacest::print_interface_token(interface_key_t key, justification just) {
  short o_screenf = gps.screenf, o_screenb = gps.screenb, o_screenbright = gps.screenbright;
  gps.changecolor(2, 0, 1);
  ::std::string tok = enabler.GetKeyDisplay(key);
  gps.addst(tok, just);
  gps.changecolor(o_screenf, o_screenb, o_screenbright);
}

interfacest::interfacest() {
  original_fps = 0;
  shutdown_interface_for_ms = 0;
  shutdown_interface_tickcount = 0;
  flag = 0;
  supermovie_on = 0;
  supermovie_pos = 0;
  supermovie_delayrate = 0;
}

interfacest::~interfacest() {
  //GO AHEAD
  while (view.child != NULL) {
    removescreen(view.child);
  }
}
