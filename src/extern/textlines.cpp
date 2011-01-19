/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/textlines.hpp"

#include <fstream>
#include <string>

void textlinesst::load_raw_to_lines(char *filename) {
  clean_lines();

  //LOAD THE FILE
  ::std::ifstream fseed(filename);
  if (fseed.is_open()) {
    ::std::string str;

    while (::std::getline(fseed, str)) {
      long end = str.length();

      while (end > 0) {
        if (isspace(str[end - 1]))
          end--;
        else
          break;
      }

      str.resize(end);

      if (str.length() > 0)
        text.add_string(str);
    }
  }
  fseed.close();
}
