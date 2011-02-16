/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/graphics.hpp"

#include "hidden/dwarf.hpp"
#include "hidden/global.hpp"
#include "extern/init.hpp"
#include "extern/texture_handler.hpp"
#include "extern/textlines.hpp"
#include "extern/enabler.hpp"

#include "intern/utils/filesystem.hpp"
#include "intern/ttf_manager.hpp"

#include "intern/i18n.hpp"
#include "intern/config.hpp"

#include <cstring>

void graphicst::addcoloredst(const char *str, const char *colorstr) {
  ::i18n::out << str;
  const int slen = strlen(str);
  int s;
  for (s = 0; s < slen && screenx < init.display.grid_x; s++) {
    if (screenx < 0) {
      s -= screenx;
      screenx = 0;
      if (s >= slen)
        break;
    }

    changecolor((colorstr[s] & 7), ((colorstr[s] & 56)) >> 3, ((colorstr[s] & 64)) >> 6);
    addchar(str[s]);
  }
}

void graphicst::addst(const ::std::string &str, justification just) {
  ::i18n::out << str;
  if (just == justify_cont)
    just = justify_left;
  if (just != not_truetype && ttf_manager.was_init()) {
    struct ttf_id id = { str, screenf, screenb, screenbright, just };
    ::std::pair< int, int > handleAndWidth = ttf_manager.get_handle(id);
    const int handle = handleAndWidth.first;
    const int width = handleAndWidth.second;
    int ourx;
    // cout << str.size() << " " << width << endl;
    switch (just) {
      case justify_center:
        ourx = screenx + (str.size() - width) / 2;
        break;
      case justify_right:
        ourx = screenx + (str.size() - width);
        break;
      default:
        ourx = screenx;
        break;
    }
    unsigned char * const s = screen + ourx * dimy * 4 + screeny * 4;
    s[0] = (handle >> 16) & 0xff;
    s[1] = (handle >> 8) & 0xff;
    s[2] = handle & 0xff;
    s[3] = GRAPHICSTYPE_TTF;
    // Also set the other tiles this text covers
    for (int x = 1; x < width; ++x) {
      *(s + x * dimy * 4 + 0) = (handle >> 16) & 0xff;
      *(s + x * dimy * 4 + 1) = (handle >> 8) & 0xff;
      *(s + x * dimy * 4 + 2) = handle & 0xff;
      *(s + x * dimy * 4 + 3) = GRAPHICSTYPE_TTFCONT;
    }
    screenx = ourx + width;
  } else {

    for (::std::size_t s = 0; s < str.length() && screenx < init.display.grid_x; s++) {
      if (screenx < 0) {
        s -= screenx;
        screenx = 0;
        if (s >= str.length())
          break;
      }

      addchar(str[s]);
    }
  }
}

void graphicst::addst(const char *str, justification just) {
  ::std::string s(str);
  addst(s, just);
}

void graphicst::erasescreen() {
  memset(screen, 0, dimx * dimy * 4);

  memset(screentexpos, 0, dimx * dimy * sizeof(long));
}

void graphicst::setclipping(long x1, long x2, long y1, long y2) {
  if (x1 < 0)
    x1 = 0;
  if (x2 > init.display.grid_x - 1)
    x2 = init.display.grid_x - 1;
  if (y1 < 0)
    y1 = 0;
  if (y2 > init.display.grid_y - 1)
    y2 = init.display.grid_y - 1;

  clipx[0] = x1;
  clipx[1] = x2;
  clipy[0] = y1;
  clipy[1] = y2;
}

void graphicst::add_tile(long texp, char addcolor) {
  if (screenx >= clipx[0] && screenx <= clipx[1] && screeny >= clipy[0] && screeny <= clipy[1]) {
    screentexpos[screenx * dimy + screeny] = texp;
    screentexpos_addcolor[screenx * dimy + screeny] = addcolor;
    screentexpos_grayscale[screenx * dimy + screeny] = 0;
  }
}

void graphicst::add_tile_grayscale(long texp, char cf, char cbr) {
  if (screenx >= clipx[0] && screenx <= clipx[1] && screeny >= clipy[0] && screeny <= clipy[1]) {
    screentexpos[screenx * dimy + screeny] = texp;
    screentexpos_addcolor[screenx * dimy + screeny] = 0;
    screentexpos_grayscale[screenx * dimy + screeny] = 1;
    screentexpos_cf[screenx * dimy + screeny] = cf;
    screentexpos_cbr[screenx * dimy + screeny] = cbr;
  }
}

void graphicst::prepare_graphics(::std::string &src_dir) {
  config const& conf = config::instance();
  if (conf.texture().use_graphics) {
    return;
  }

  texture.clean();

  //GET READY TO LOAD
  svector< char * > processfilename;
  textlinesst setuplines;
  char str[400];

  //LOAD THE OBJECT FILES UP INTO MEMORY
  //MUST INSURE THAT THEY ARE LOADED IN THE PROPER ORDER, IN CASE THEY REFER TO EACH OTHER
  ::std::string chk = src_dir;
  chk += "graphics/graphics_*";

  ::std::vector< ::std::string > files;
  find_files_by_glob(chk, files, "text");

  processfilename = files;

  ::std::string chktype = "GRAPHICS";
  for (::std::size_t f = 0; f < processfilename.size(); f++) {
    strcpy(str, src_dir.c_str());
    strcat(str, "graphics/");
    strcat(str, processfilename[f]);
    setuplines.load_raw_to_lines(str);

    errorlog_prefix = "*** Error(s) found in the file \"";
    errorlog_prefix += str;
    errorlog_prefix += '\"';
    process_object_lines(setuplines, chktype, src_dir);
    errorlog_prefix.clear();

    delete[] processfilename[f];
  }
  processfilename.clear();

  enabler.reset_textures();
}

void graphicst::dim_colors(long x, long y, char dim) {
  if (x >= clipx[0] && x <= clipx[1] && y >= clipy[0] && y <= clipy[1]) {
    switch (dim) {
      case 4:
        switch (screen[x * dimy * 4 + y * 4 + 2]) {
          case 4:
          case 5:
          case 6:
            screen[x * dimy * 4 + y * 4 + 2] = 1;
            break;
          case 2:
          case 7:
            screen[x * dimy * 4 + y * 4 + 2] = 3;
            break;
        }
        switch (screen[x * dimy * 4 + y * 4 + 1]) {
          case 4:
          case 5:
          case 6:
            screen[x * dimy * 4 + y * 4 + 1] = 1;
            break;
          case 2:
          case 7:
            screen[x * dimy * 4 + y * 4 + 1] = 3;
            break;
        }
        if (screen[x * dimy * 4 + y * 4 + 1] == screen[x * dimy * 4 + y * 4 + 2])
          screen[x * dimy * 4 + y * 4 + 1] = 0;
        screen[x * dimy * 4 + y * 4 + 3] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == 0 && screen[x * dimy * 4 + y * 4 + 2] == 0 && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 3] = 1;
        break;
      case 3:
        switch (screen[x * dimy * 4 + y * 4 + 2]) {
          case 4:
          case 5:
            screen[x * dimy * 4 + y * 4 + 2] = 6;
            break;
          case 2:
          case 7:
            screen[x * dimy * 4 + y * 4 + 2] = 3;
            break;
        }
        switch (screen[x * dimy * 4 + y * 4 + 1]) {
          case 1:
            screen[x * dimy * 4 + y * 4 + 3] = 0;
            break;
          case 4:
          case 5:
            screen[x * dimy * 4 + y * 4 + 1] = 6;
            break;
          case 2:
            screen[x * dimy * 4 + y * 4 + 1] = 3;
            break;
          case 7:
            screen[x * dimy * 4 + y * 4 + 1] = 3;
            break;
        }
        if (screen[x * dimy * 4 + y * 4 + 1] != 7)
          screen[x * dimy * 4 + y * 4 + 3] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == screen[x * dimy * 4 + y * 4 + 2] && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 1] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == 0 && screen[x * dimy * 4 + y * 4 + 2] == 0 && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 3] = 1;
        break;
      case 2:
        switch (screen[x * dimy * 4 + y * 4 + 2]) {
          case 4:
          case 5:
            screen[x * dimy * 4 + y * 4 + 2] = 6;
            break;
        }
        switch (screen[x * dimy * 4 + y * 4 + 1]) {
          case 4:
          case 5:
            screen[x * dimy * 4 + y * 4 + 1] = 6;
            break;
        }
        if (screen[x * dimy * 4 + y * 4 + 1] != 7)
          screen[x * dimy * 4 + y * 4 + 3] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == screen[x * dimy * 4 + y * 4 + 2] && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 1] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == 0 && screen[x * dimy * 4 + y * 4 + 2] == 0 && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 3] = 1;
        break;
      case 1:
        if (screen[x * dimy * 4 + y * 4 + 1] != 7)
          screen[x * dimy * 4 + y * 4 + 3] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == screen[x * dimy * 4 + y * 4 + 2] && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 1] = 0;
        if (screen[x * dimy * 4 + y * 4 + 1] == 0 && screen[x * dimy * 4 + y * 4 + 2] == 0 && screen[x * dimy * 4 + y * 4 + 3] == 0)
          screen[x * dimy * 4 + y * 4 + 3] = 1;
        break;
    }
  }
}

void graphicst::color_square(long x, long y, unsigned char f, unsigned char b, unsigned char br) {
  if (x >= clipx[0] && x <= clipx[1] && y >= clipy[0] && y <= clipy[1]) {
    screen[x * dimy * 4 + y * 4 + 1] = f;
    screen[x * dimy * 4 + y * 4 + 2] = b;
    screen[x * dimy * 4 + y * 4 + 3] = br;
  }
}

void graphicst::get_mouse_text_coords(int32_t &mx, int32_t &my) {
  mx = mouse_x;
  my = mouse_y;
}
