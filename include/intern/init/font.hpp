/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_INIT_FONT_HPP_
#define INTERN_INIT_FONT_HPP_

class init_fontst {
  public:
    long small_font_texpos[256];
    long large_font_texpos[256];
    long small_font_datapos[256];
    long large_font_datapos[256];
    float small_font_adjx;
    float small_font_adjy;
    float large_font_adjx;
    float large_font_adjy;
    long small_font_dispx;
    long small_font_dispy;
    long large_font_dispx;
    long large_font_dispy;
    bool use_ttf;
};

#endif /* INTERN_INIT_FONT_HPP_ */
