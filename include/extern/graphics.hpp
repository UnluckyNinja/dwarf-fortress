/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_GRAPHICS_HPP_
#define EXTERN_GRAPHICS_HPP_

#include <map>

#include <string>
#include "integer_types.hpp"
#include "utils/strings.hpp"

typedef unsigned long DWORD;

typedef long LONG;
typedef long long LONGLONG;

typedef union {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

enum Texture {
  TEXTURE_MOUSE, TEXTURENUM
};

/* screen array layout
 *
 *
 * X*Y tiles of 4 bytes each in column-major order (FIXME: This is inefficient! It should be row-major!)
 * For each tile, byte 0 is the character, 1 is foreground color, 2 is bacground, and 3 denotes bold.
 *
 * As there are only 8 different colors and bold is a boolean, this leaves a lot of free space. Therefore,
 * without involving the graphics arrays, out-of-gamut values can be used for designating TTF objects.
 *
 * This means setting the bold byte to all 1s (0xff), and then using the other three bytes as a designator.
 *
 * Time will tell whether this was a good idea or not.
 */

// So yeah, we store "type" in the previously bold byte. This means we have quite a lot of free types yet.
#define GRAPHICSTYPE_TTF 0xff
// This type denotes a tile that is covered by truetype, but is not the tile it starts on.
#define GRAPHICSTYPE_TTFCONT 0xfe

class graphicst {
    int lookup_pair(::std::pair< int, int > color);
    long calculate_old_fps();
  public:
    long screenx, screeny;
    char screenf, screenb;
    char screenbright;

    unsigned char *screen;
    long *screentexpos;
    char *screentexpos_addcolor;
    unsigned char *screentexpos_grayscale;
    unsigned char *screentexpos_cf;
    unsigned char *screentexpos_cbr;

    // Calling this is not enough in itself. You also need to call swap_front/back.
    void resize(int x, int y);

    long clipx[2], clipy[2];
    long tex_pos[TEXTURENUM];

    long rect_id;

    LARGE_INTEGER print_time[100];
    long print_index;
    char display_frames;

    short force_full_display_count;

    char original_rect;

    int dimx, dimy;

    graphicst();

    void locate(long y, long x);
    void changecolor(short f, short b, char bright);
    void addchar(unsigned char c, char advance = 1);
    void addchar(unsigned int x, unsigned int y, unsigned char c, unsigned char f, unsigned char b, unsigned char bright);

    /**
     * @ingroup extern
     */
    void addcoloredst(const char *str, const char *colorstr);

    // TODO: comment
    // TODO: categorize
    /**
     * @ingroup extern
     */
    void addst(const ::std::string &str, justification just = justify_left);

    /**
     * @ingroup extern
     */
    void addst(const char *str, justification just = justify_left);

    void erasescreen_clip();
    /**
     * @ingroup extern
     */
    void erasescreen();
    void erasescreen_rect(int x1, int x2, int y1, int y2);
    /**
     * @ingroup extern
     */
    void setclipping(long x1, long x2, long y1, long y2);

    /**
     * @ingroup extern
     */
    void add_tile(long texp, char addcolor);
    /**
     * @ingroup extern
     */
    void add_tile_grayscale(long texp, char cf, char cbr);

    /**
     * @ingroup extern
     */
    void prepare_graphics(::std::string &src_dir);

    void gray_out_rect(long sx, long ex, long sy, long ey);
    /**
     * @ingroup extern
     */
    void dim_colors(long x, long y, char dim);

    void rain_color_square(long x, long y);
    void snow_color_square(long x, long y);
    /**
     * @ingroup extern
     */
    void color_square(long x, long y, unsigned char f, unsigned char b, unsigned char br);

    long border_start_x();
    long border_start_y();
    long border_end_x();
    long border_end_y();
    long text_width();
    long text_height();
    long window_element_height(long minus, char border);

    int mouse_x, mouse_y;
    /**
     * @ingroup extern
     */
    void get_mouse_text_coords(int32_t &mx, int32_t &my);
    void draw_border(int x1, int x2, int y1, int y2);

    // Instead of doing costly bounds-checking calculations, we cache the end
    // of the arrays..
    unsigned char *screen_limit;
};

/**
 * @ingroup extern
 */
extern graphicst gps;

#endif /* EXTERN_GRAPHICS_HPP_ */
