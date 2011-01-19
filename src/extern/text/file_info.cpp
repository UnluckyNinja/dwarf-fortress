/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/text/file_info.hpp"

#include "intern/text/text_info.hpp"
#include "intern/text/text_info_element_long.hpp"
#include "intern/text/text_info_element_string.hpp"
#include "intern/utils/random.hpp"

#include <logging/logging.hpp>

#include <fstream>

void text_system_file_infost::initialize_info() {
  ::std::ifstream fseed(filename.c_str());
  if (fseed.is_open()) {
    ::std::string str;

    while (::std::getline(fseed, str)) {
      if (str.length() > 0)
        number++;
    }
  } else {
    __error
      << "Error Initializing Text: " << filename;
  }
  fseed.close();
}

void text_system_file_infost::get_text(text_infost& text) {
  text.clean();

  if (number == 0)
    return;

  ::std::ifstream fseed(filename.c_str());
  if (fseed.is_open()) {
    ::std::string str;

    int num = get_random_int(number);

    //SKIP AHEAD TO THE RIGHT SPOT
    while (num > 0) {
      ::std::getline(fseed, str);
      num--;
    }

    //PROCESS THE STRING INTO TEXT ELEMENTS
    if (::std::getline(fseed, str)) {
      int curpos;
      ::std::string nextstr;
      char doing_long = 0;

      text_info_elementst *newel;
      long end = str.length();

      while (end > 0) {
        if (isspace(str[end - 1]))
          end--;
        else
          break;
      }

      str.resize(end);

      for (curpos = 0; curpos < end; curpos++) {
        //HANDLE TOKEN OR ENDING
        //TWO FILE TOKENS IN A ROW MEANS LONG
        //ONE MEANS STRING
        if (str[curpos] == file_token || curpos == end - 1) {
          if (str[curpos] != file_token)
            nextstr += str[curpos];

          //HAVE SOMETHING == SAVE IT
          if (!nextstr.empty()) {
            if (doing_long) {
              newel = new text_info_element_longst(atoi(nextstr.c_str()));
              text.element.push_back(newel);
              doing_long = 0;
            } else {
              newel = new text_info_element_stringst(nextstr);
              text.element.push_back(newel);
            }

            nextstr.erase();
          }
          //STARTING A LONG
          else {
            doing_long = 1;
          }
        }
        //JUST ADD IN ANYTHING ELSE
        else {
          nextstr += str[curpos];
        }
      }
    }
  }
  fseed.close();
}
