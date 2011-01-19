/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_ENABLER_HPP_
#define EXTERN_ENABLER_HPP_

#include "intern/utils/opengl.hpp"

#include "extern/enabler/input.hpp"
#include "extern/textures.hpp"

#include "intern/text_system.hpp"

#include "intern/mutex_variable.hpp"

#include <stack>
#include <queue>

#include <string>
#include "integer_types.hpp"

struct renderer;

#define GAME_TITLE_STRING "Dwarf Fortress"

#define MAX_GRID_X 256
#define MAX_GRID_Y 256
#define MIN_GRID_X 80
#define MIN_GRID_Y 25

enum zoom_commands {
  zoom_in, zoom_out, zoom_reset, zoom_fullscreen, zoom_resetgrid
};

#define ENABLERFLAG_RENDER (1 << 0)
#define ENABLERFLAG_MAXFPS (1 << 1)

/**
 * @ingroup extern
 */
class enablerst: public enabler_inputst {
    friend class initst;
    friend class renderer_2d_base;
    friend class renderer_2d;
    friend class renderer_opengl;
    friend class renderer_curses;

    bool fullscreen;
    ::std::stack< ::std::pair< int, int > > overridden_grid_sizes;

    class renderer *renderer;
    void eventLoop_SDL();
#ifdef CURSES
    void eventLoop_ncurses();
#endif

    // Framerate calculations
    int calculated_fps, calculated_gfps;
    ::std::queue< int > frame_timings, gframe_timings; // Milisecond lengths of the last few frames
    int frame_sum, gframe_sum;
    int frame_last, gframe_last; // SDL_GetTick returns
    void do_update_fps(::std::queue< int > &q, int &sum, int &last, int &calc);
  public:
    void clear_fps();
  private:
    void update_fps();
    void update_gfps();

    // Frame timing calculations
    float fps, gfps;
    float fps_per_gfps;
    uint32_t last_tick;
    float outstanding_frames, outstanding_gframes;

    // Async rendering
    struct async_cmd {
        enum cmd_t {
          pause, start, render, inc, set_fps
        } cmd;
        int val; // If async_inc, number of extra frames to run. If set_fps, current value of fps.
        async_cmd() {
        }
        async_cmd(cmd_t c) {
          cmd = c;
        }
    };

    struct async_msg {
        enum msg_t {
          quit, complete, set_fps, set_gfps, push_resize, pop_resize, reset_textures
        } msg;
        union {
            int fps; // set_fps, set_gfps
            struct { // push_resize
                int x, y;
            };
        };
        async_msg() {
        }
        async_msg(msg_t m) {
          msg = m;
        }
    };

    unsigned int async_frames; // Number of frames the async thread has been asked to run
    bool async_paused;
    Chan< async_cmd > async_tobox; // Messages to the simulation thread
    Chan< async_msg > async_frombox; // Messages from the simulation thread, and acknowledgements of those to
    Chan< zoom_commands > async_zoom; // Zoom commands (from the simulation thread)
    Chan< void > async_fromcomplete; // Barrier for async_msg requests that require acknowledgement
  public:
    uint32_t renderer_threadid;
  private:

    void pause_async_loop();
    void async_wait();
    void unpause_async_loop();

  public:

    ::std::string command_line;

    float ccolor[16][3]; // The curses-RGB mapping used for non-curses display modes

    enablerst();
    unsigned long flag; // ENABLERFLAG_RENDER, ENABLERFLAG_MAXFPS

    int loop(::std::string cmdline);
    void async_loop();
    void do_frame();

    // Framerate interface
    void set_fps(int fps);
    void set_gfps(int gfps);
    int get_fps();
    int get_gfps();
    int calculate_fps(); // Calculate the actual provided (G)FPS
    int calculate_gfps();

    // Mouse interface, such as it is
    char mouse_lbut, mouse_rbut, mouse_lbut_down, mouse_rbut_down, mouse_lbut_lift, mouse_rbut_lift;
    char tracking_on; // Whether we're tracking the mouse or not

    // OpenGL state (wrappers)
    class textures textures; // Font/graphics texture catalog
    GLsync sync; // Rendering barrier
    void reset_textures();
    bool uses_opengl();

    // Grid-size interface
    void override_grid_size(int w, int h); // Pick a /particular/ grid-size
    void release_grid_size(); // Undoes override_grid_size
    void zoom_display(zoom_commands command);

    // Window management
    bool is_fullscreen();
    void toggle_fullscreen();

    // Conversations
    text_systemst text;

    // TOADY: MOVE THESE TO "FRAMERATE INTERFACE"
    MVar< int > simticks, gputicks;

    // An *approximation* of the current time for use in garbage collection thingies, updated every frame or so.
    Uint32 clock;

    //  protected:
    //    int simticks;
    //    ::boost::mutex simticks_mutex;

  public:
    void set_simticks(int simticks_in);
    int inc_simticks();
    int get_simticks();

    //  protected:
    //    int gputicks;
    //    ::boost::mutex gputicks_mutex;

  public:
    void set_gputicks(int gputicks_in);
    int inc_gputicks();
    int get_gputicks();
};

/**
 * @ingroup extern
 */
extern enablerst enabler;

#endif /* EXTERN_ENABLER_HPP_ */
