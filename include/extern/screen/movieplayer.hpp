/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_SCREEN_MOVIEPLAYER_HPP_
#define EXTERN_SCREEN_MOVIEPLAYER_HPP_

#include <set>

#include <string>
#include "integer_types.hpp"
#include "event_types.hpp"

#include "extern/svector.hpp"

#include "hidden/viewscreen.hpp"

class viewscreen_movieplayerst: viewscreenst {
  public:
    /**
     * @ingroup extern
     */
    static viewscreen_movieplayerst* create(char pushtype, viewscreenst* scr = 0);
    virtual void help();
    virtual void feed(::std::set< interface_key_t > &events);
    virtual void logic();
    virtual void render();
    virtual char movies_okay();
    void clearfilelist();
    /**
     * @ingroup extern
     */
    void force_play(const ::std::string& file);

    virtual char is_option_screen();

  protected:
    char saving;
    char loading;
    char editing;
    char text_mode;
    unsigned char editing_char;
    int editing_copy_from;
    short editing_screenf;
    short editing_screenb;
    short editing_screenbright;
    int editing_selected_sound;
    char editing_menu;
    ::std::string savename;
    ::std::string force_file;
    char is_playing;
    char is_forced_play;
    char quit_if_no_play;
    int maxmoviepos;
    int end_frame_pos;

    int32_t selfile;
    svector< char * > filelist;

    viewscreen_movieplayerst();
    virtual ~viewscreen_movieplayerst();
};

#endif /* EXTERN_SCREEN_MOVIEPLAYER_HPP_ */
