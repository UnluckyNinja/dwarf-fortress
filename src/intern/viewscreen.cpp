/**
 * @file
 * @date 26 juin 2010
 * @todo comment
 */

#include "hidden/viewscreen.hpp"

#include "extern/interface.hpp"

void viewscreenst::feed(::std::set< interface_key_t >& events) {
}

void viewscreenst::logic() {
}

void viewscreenst::render() {
}

char viewscreenst::is_option_screen() {
  return 0;
}

viewscreenst::viewscreenst() {
  child = 0;
  parent = 0;
  breakdownlevel = INTERFACE_BREAKDOWN_NONE;
  option_key_pressed = 0;
}

viewscreenst::~viewscreenst() {
}
