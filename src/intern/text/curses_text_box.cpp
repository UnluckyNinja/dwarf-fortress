/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/text/curses_text_box.hpp"

#include "intern/pstring.hpp"

void curses_text_boxst::add_paragraph(stringvectst &src, int32_t para_width) {
  bool skip_leading_spaces = false;

  //ADD EACH OF THE STRINGS ON IN TURN
  ::std::string curstr;
  //  long strlength = 0;
  long s, pos;
  for (s = 0; s < src.str.size(); s++) {
    //GRAB EACH WORD, AND SEE IF IT FITS, IF NOT START A NEW LINE
    for (pos = 0; pos < src.str[s]->dat.size(); pos++) {
      if (skip_leading_spaces) {
        if (src.str[s]->dat[pos] == ' ')
          continue;
        else
          skip_leading_spaces = false;
      }

      //ADD TO WORD
      curstr += src.str[s]->dat[pos];

      //IF TOO LONG, CUT BACK TO FIRST SPACE
      if (curstr.length() > para_width) {
        long opos = pos;

        long minus = 0;
        do {
          pos--;
          minus++;
        } while (src.str[s]->dat[pos] != ' ' && pos > 0);

        //IF WENT ALL THE WAY BACK, INTRODUCE A SPACE
        if (minus == curstr.size()) {
          src.str[s]->dat.insert(opos - 1, " ");
        } else {
          curstr.resize(curstr.size() - minus);
          text.add_string(curstr);
          skip_leading_spaces = true;
        }
        curstr.erase();
      }
    }
  }

  //FLUSH FINAL BIT
  if (!curstr.empty())
    text.add_string(curstr);
}

void curses_text_boxst::read_file(file_compressorst &filecomp, int32_t loadversion) {
  text.read_file(filecomp, loadversion);
}

void curses_text_boxst::write_file(file_compressorst &filecomp) {
  text.write_file(filecomp);
}

void curses_text_boxst::clean() {
  text.clean();
}
