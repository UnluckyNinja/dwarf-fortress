//some of this stuff is based on public domain code from nehe or opengl books over the years
//additions and modifications Copyright (c) 2008, Tarn Adams
//All rights reserved.  See game.cpp or license.txt for more information.

#ifndef ENABLER_H
#define ENABLER_H

#include "extern/text/file_info.hpp"

#include "platform.h"
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#ifdef __APPLE__
# include <SDL_image/SDL_image.h>
#else
# include <SDL/SDL_image.h>
#endif

#include "GL/glew.h"

#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <list>
#include <iostream>
#include <sstream>
#include <stack>
#include <queue>
#include <fstream>

using std::vector;
using std::pair;
using std::map;
using std::list;
using std::stack;
using std::queue;

#include "endian.h"
#include "mail.hpp"

#undef CURSES

#define ENABLER

#ifndef BITS

#define BITS

#define BIT1 1
#define BIT2 2
#define BIT3 4
#define BIT4 8
#define BIT5 16
#define BIT6 32
#define BIT7 64
#define BIT8 128
#define BIT9 256
#define BIT10 512
#define BIT11 1024
#define BIT12 2048
#define BIT13 4096
#define BIT14 8192
#define BIT15 16384
#define BIT16 32768
#define BIT17 65536UL
#define BIT18 131072UL
#define BIT19 262144UL
#define BIT20 524288UL
#define BIT21 1048576UL
#define BIT22 2097152UL
#define BIT23 4194304UL
#define BIT24 8388608UL
#define BIT25 16777216UL
#define BIT26 33554432UL
#define BIT27 67108864UL
#define BIT28 134217728UL
#define BIT29 268435456UL
#define BIT30 536870912UL
#define BIT31 1073741824UL
#define BIT32 2147483648UL

#endif

#include "extern/string_vect.hpp"

#ifdef ENABLER

#undef COLOR_BLUE
#undef COLOR_CYAN
#undef COLOR_RED
#undef COLOR_YELLOW

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_YELLOW 6
#define COLOR_WHITE	7

enum ColorData {
  COLOR_DATA_WHITE_R,
  COLOR_DATA_WHITE_G,
  COLOR_DATA_WHITE_B,
  COLOR_DATA_RED_R,
  COLOR_DATA_RED_G,
  COLOR_DATA_RED_B,
  COLOR_DATA_GREEN_R,
  COLOR_DATA_GREEN_G,
  COLOR_DATA_GREEN_B,
  COLOR_DATA_BLUE_R,
  COLOR_DATA_BLUE_G,
  COLOR_DATA_BLUE_B,
  COLOR_DATA_YELLOW_R,
  COLOR_DATA_YELLOW_G,
  COLOR_DATA_YELLOW_B,
  COLOR_DATA_MAGENTA_R,
  COLOR_DATA_MAGENTA_G,
  COLOR_DATA_MAGENTA_B,
  COLOR_DATA_CYAN_R,
  COLOR_DATA_CYAN_G,
  COLOR_DATA_CYAN_B,
  COLOR_DATANUM
};

#define TILEFLAG_DEAD BIT1
#define TILEFLAG_ROTATE BIT2
#define TILEFLAG_PIXRECT BIT3
#define TILEFLAG_HORFLIP BIT4
#define TILEFLAG_VERFLIP BIT5
#define TILEFLAG_LINE BIT6
#define TILEFLAG_RECT BIT7
#define TILEFLAG_BUFFER_DRAW BIT8
#define TILEFLAG_MODEL_PERSPECTIVE BIT9
#define TILEFLAG_MODEL_ORTHO BIT10
#define TILEFLAG_MODEL_TRANSLATE BIT11
#define TILEFLAG_LINE_3D BIT12

#define TRIMAX 9999

enum render_phase {
  setup, // 0
  complete,
  phase_count
};

#define COPYTEXTUREFLAG_HORFLIP BIT1
#define COPYTEXTUREFLAG_VERFLIP BIT2

struct tile {
    int x, y;
    long tex;
};

typedef struct { // Window Creation Info
    char* title; // Window Title
    int width; // Width
    int height; // Height
    int bitsPerPixel; // Bits Per Pixel
    BOOL isFullScreen; // FullScreen?
} GL_WindowInit; // GL_WindowInit

typedef struct { // Contains Information Vital To A Window
    GL_WindowInit init; // Window Init
    BOOL isVisible; // Window Visible?
} GL_Window; // GL_Window

#include "extern/enabler/input.hpp"

#endif

#endif //ENABLER_H
