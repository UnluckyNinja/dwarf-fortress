/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include "extern/enabler/input.hpp"

#include <map>
#include <set>

#include "intern/enabler/input.hpp"
#include <logging/logging.hpp>

#include "intern/keybindings.hpp"

::std::string enabler_inputst::GetKeyDisplay(int32_t binding) {
  enabler_input& input = enabler_input::instance();

  ::std::map< interface_key_t, ::std::set< ::std::string, less_sz > >::iterator it = input.get_keydisplay().find(binding);

  if (it != input.get_keydisplay().end() && it->second.size() > 0) {
    return *(it->second.begin());
  } else {
    __info << "Missing binding displayed: " + bindingNames.left.at(binding);
    return "?";
  }
}

::std::string enabler_inputst::GetBindingDisplay(int32_t binding) {
  ::boost::bimap< interface_key_t, ::std::string >::left_iterator it = bindingNames.left.find(binding);

  if (it != bindingNames.left.end()) {
    return it->second;
  } else {
    return "NO BINDING";
  }
}
