/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/screen/movieplayer.hpp"

#include "hidden/global.hpp"

#include "extern/interface.hpp"

viewscreen_movieplayerst* viewscreen_movieplayerst::create(char pushtype, viewscreenst* scr) {
  viewscreen_movieplayerst* newv = new viewscreen_movieplayerst();
  gview.addscreen(newv, pushtype, scr);

  return newv;
}

void viewscreen_movieplayerst::force_play(const ::std::string& file) {
  force_file = file;
  is_forced_play = 1;
}
