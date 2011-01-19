/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_INTERFACE_HPP_
#define EXTERN_INTERFACE_HPP_

#include "hidden/viewscreen.hpp"

#include "extern/string_vect.hpp"
#include "extern/musicsound.hpp"

#include "extern/utils/strings.hpp"

#define MOVIEBUFFSIZE 800000u
#define COMPMOVIEBUFFSIZE 1000000u

/**
 * @brief \a interfacest::flag flag.
 */
#define INTERFACEFLAG_RETAIN_NONZERO_INPUT (1 << 0)

enum InterfacePushType {
  INTERFACE_PUSH_AS_PARENT, INTERFACE_PUSH_AS_CHILD, INTERFACE_PUSH_AT_BACK, INTERFACE_PUSH_AT_FRONT, INTERFACE_PUSHNUM
};

enum InterfaceBreakdownTypes {
  INTERFACE_BREAKDOWN_NONE, INTERFACE_BREAKDOWN_QUIT, INTERFACE_BREAKDOWN_STOPSCREEN, INTERFACE_BREAKDOWN_TOFIRST, INTERFACE_BREAKDOWNNUM
};

class interfacest {
    int original_fps;
    viewscreenst* grab_lastscreen();
    friend class viewscreen_movieplayerst;

  public:
    viewscreenst view;
    unsigned int flag;

    int shutdown_interface_tickcount;
    int shutdown_interface_for_ms;

    /**
     * @ingroup extern
     */
    char loop();
    void remove_to_first();
    void removescreen(viewscreenst *scr);
    /**
     * @ingroup extern
     */
    void addscreen(viewscreenst *scr, char pushtype, viewscreenst *relate);
    char is_supermovie_on();

    /**
     * @ingroup extern
     */
    void print_interface_token(interface_key_t key, justification just = justify_left);

    /**
     * @ingroup extern
     */
    interfacest();
    /**
     * @ingroup extern
     */
    ~interfacest();

  protected:
    char supermovie_on;
    int supermovie_pos;
    int supermovie_delayrate;
    int supermovie_delaystep;
    stringvectst supermovie_sound;
#ifndef NO_FMOD
    int supermovie_sound_time[200][SOUND_CHANNELNUM];
#endif
    unsigned char supermoviebuffer[MOVIEBUFFSIZE];
    unsigned char supermoviebuffer_comp[COMPMOVIEBUFFSIZE];
    int currentblocksize;
    int nextfilepos;
    char first_movie_write;
    ::std::string movie_file;

    void insertscreen_as_parent(viewscreenst *scr, viewscreenst *child);
    void insertscreen_as_child(viewscreenst *scr, viewscreenst *parent);
    void insertscreen_at_back(viewscreenst *scr);
    void insertscreen_at_front(viewscreenst *scr);
    void handlemovie(char flushall);
    void finish_movie();
    void use_movie_input();

    int write_movie_chunk();
    void read_movie_chunk(int &maxmoviepos, char &is_playing);
};

#endif /* EXTERN_INTERFACE_HPP_ */
