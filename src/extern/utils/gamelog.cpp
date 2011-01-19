/**
 * @file
 * @date 6 juin 2010
 * @todo comment
 */

#include "extern/utils/gamelog.hpp"

#include "hidden/global.hpp"

#include <logging/logging.hpp>

void errorlog_string(const char *message_ptr) {
  __error
    << errorlog_prefix << message_ptr;
  errorlog_prefix.clear();
}

void errorlog_string(const ::std::string& message) {
  errorlog_string(message.c_str());
}

void gamelog_string(const ::std::string& message) {
  __info
    << message;
}
