/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/text/file_info.hpp"

text_system_file_infost* text_system_file_infost::add_file_info(const ::std::string& newf, long newi, char newft) {
  return new text_system_file_infost(newf, newi, newft);
}

text_system_file_infost::text_system_file_infost(const ::std::string &newf, long newi, char newft) {
  filename = newf;
  file_token = newft;
  index = newi;
  number = 0;
}
