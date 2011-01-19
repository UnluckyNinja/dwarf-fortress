/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_SCREEN_WIDGET_TEXTBOX_HPP_
#define INTERN_SCREEN_WIDGET_TEXTBOX_HPP_

#include <set>

#include <string>
#include "event_types.hpp"

namespace widgets {
  class textbox {
      ::std::string text;
      bool keep;
    public:
      textbox();
      textbox(::std::string initializer, bool keep);
      ::std::string get_text();

      // Only cares about INTERFACEKEY_STRING events
      void feed(::std::set< interface_key_t > &input);
      void render(int x1, int x2, int y1, int y2);
  };
}

#endif /* INTERN_SCREEN_WIDGET_TEXTBOX_HPP_ */
