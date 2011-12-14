#include <cassert>

#include "platform.h"
#include "enabler.h"
#include "random.h"
#include "init.h"
#include "music_and_sound_g.h"

#ifdef unix
# include <locale.h>
#endif

using namespace std;

enablerst enabler;


// For the printGLError macro
int glerrorcount = 0;

// Set to 0 when the game wants to quit
int loopvar = 1;

// Reports an error to the user, using a MessageBox and stderr.
void report_error(const char *error_preface, const char *error_message)
{
  char *buf = NULL;
  // +4 = +colon +space +newline +nul
  buf = new char[strlen(error_preface) + strlen(error_message) + 4];
  sprintf(buf, "%s: %s\n", error_preface, error_message);
  MessageBox(NULL, buf, "Error", MB_OK);
  fprintf(stderr, "%s", buf);
  delete [] buf;
}

Either<texture_fullid,texture_ttfid> renderer::screen_to_texid(int x, int y) {
  const int tile = x * gps.dimy + y;
  const unsigned char *s = screen + tile*4;

  // TTF text does not get the full treatment.
  if (s[3] == GRAPHICSTYPE_TTF) {
    texture_ttfid texpos = (s[0] << 16) | (s[1] << 8) | s[2];
    return Either<texture_fullid,texture_ttfid>(texpos);
  } else if (s[3] == GRAPHICSTYPE_TTFCONT)
    return Either<texture_fullid,texture_ttfid>(0);

  // Otherwise, it's a normal (graphical?) tile.
  struct texture_fullid ret;
  const int ch   = s[0];
  const int bold = (s[3] != 0) * 8;
  const int fg   = (s[1] + bold) % 16;
  const int bg   = s[2] % 16;

  static bool use_graphics = init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS);

  if (use_graphics) {
    const long texpos             = screentexpos[tile];
    const char addcolor           = screentexpos_addcolor[tile];
    const unsigned char grayscale = screentexpos_grayscale[tile];
    const unsigned char cf        = screentexpos_cf[tile];
    const unsigned char cbr       = screentexpos_cbr[tile];

    if (texpos) {
      ret.texpos = texpos;
      if (grayscale) {
        ret.r = enabler.ccolor[cf][0];
        ret.g = enabler.ccolor[cf][1];
        ret.b = enabler.ccolor[cf][2];
        ret.br = enabler.ccolor[cbr][0];
        ret.bg = enabler.ccolor[cbr][1];
        ret.bb = enabler.ccolor[cbr][2];
      } else if (addcolor) {
        goto use_ch;
      } else {
        ret.r = ret.g = ret.b = 1;
        ret.br = ret.bg = ret.bb = 0;
      }
      goto skip_ch;
    }
  }

  ret.texpos = enabler.is_fullscreen() ?
    init.font.large_font_texpos[ch] :
    init.font.small_font_texpos[ch];
 use_ch:
  ret.r = enabler.ccolor[fg][0];
  ret.g = enabler.ccolor[fg][1];
  ret.b = enabler.ccolor[fg][2];
  ret.br = enabler.ccolor[bg][0];
  ret.bg = enabler.ccolor[bg][1];
  ret.bb = enabler.ccolor[bg][2];

 skip_ch:

  return Either<texture_fullid,texture_ttfid>(ret);
}


enablerst::enablerst() {
  fullscreen = false;
  sync = NULL;
  renderer = NULL;
  calculated_fps = calculated_gfps = frame_sum = gframe_sum = frame_last = gframe_last = 0;
  fps = 100; gfps = 20;
  fps_per_gfps = fps / gfps;
  last_tick = 0;
}

void renderer::display()
{
  const int dimx = init.display.grid_x;
  const int dimy = init.display.grid_y;
  static bool use_graphics = init.display.flag.has_flag(INIT_DISPLAY_FLAG_USE_GRAPHICS);
  if (gps.force_full_display_count) {
    // Update the entire screen
    update_all();
  } else {
    Uint32 *screenp = (Uint32*)screen, *oldp = (Uint32*)screen_old;
    if (use_graphics) {
      int off = 0;
      for (int x2=0; x2 < dimx; x2++) {
        for (int y2=0; y2 < dimy; y2++, ++off, ++screenp, ++oldp) {
          // We don't use pointers for the non-screen arrays because we mostly fail at the
          // *first* comparison, and having pointers for the others would exceed register
          // count.
          // Partial printing (and color-conversion): Big-ass if.
          if (*screenp == *oldp &&
              screentexpos[off] == screentexpos_old[off] &&
              screentexpos_addcolor[off] == screentexpos_addcolor_old[off] &&
              screentexpos_grayscale[off] == screentexpos_grayscale_old[off] &&
              screentexpos_cf[off] == screentexpos_cf_old[off] &&
              screentexpos_cbr[off] == screentexpos_cbr_old[off])
            {
              // Nothing's changed, this clause deliberately empty
            } else {
            update_tile(x2, y2);
          }
        }
      }
    } else {
      for (int x2=0; x2 < dimx; ++x2) {
        for (int y2=0; y2 < dimy; ++y2, ++screenp, ++oldp) {
          if (*screenp != *oldp) {
            update_tile(x2, y2);
          }
        }
      }
    }
  }
  if (gps.force_full_display_count > 0) gps.force_full_display_count--;
}

void renderer::gps_allocate(int x, int y) {
  if (screen) delete[] screen;
  if (screentexpos) delete[] screentexpos;
  if (screentexpos_addcolor) delete[] screentexpos_addcolor;
  if (screentexpos_grayscale) delete[] screentexpos_grayscale;
  if (screentexpos_cf) delete[] screentexpos_cf;
  if (screentexpos_cbr) delete[] screentexpos_cbr;
  if (screen_old) delete[] screen_old;
  if (screentexpos_old) delete[] screentexpos_old;
  if (screentexpos_addcolor_old) delete[] screentexpos_addcolor_old;
  if (screentexpos_grayscale_old) delete[] screentexpos_grayscale_old;
  if (screentexpos_cf_old) delete[] screentexpos_cf_old;
  if (screentexpos_cbr_old) delete[] screentexpos_cbr_old;

  gps.screen = screen = new unsigned char[x*y*4];
  memset(screen, 0, x*y*4);
  gps.screentexpos = screentexpos = new long[x*y];
  memset(screentexpos, 0, x*y*sizeof(long));
  gps.screentexpos_addcolor = screentexpos_addcolor = new char[x*y];
  memset(screentexpos_addcolor, 0, x*y);
  gps.screentexpos_grayscale = screentexpos_grayscale = new unsigned char[x*y];
  memset(screentexpos_grayscale, 0, x*y);
  gps.screentexpos_cf = screentexpos_cf = new unsigned char[x*y];
  memset(screentexpos_cf, 0, x*y);
  gps.screentexpos_cbr = screentexpos_cbr = new unsigned char[x*y];
  memset(screentexpos_cbr, 0, x*y);

  screen_old = new unsigned char[x*y*4];
  memset(screen_old, 0, x*y*4);
  screentexpos_old = new long[x*y];
  memset(screentexpos_old, 0, x*y*sizeof(long));
  screentexpos_addcolor_old = new char[x*y];
  memset(screentexpos_addcolor_old, 0, x*y);
  screentexpos_grayscale_old = new unsigned char[x*y];
  memset(screentexpos_grayscale_old, 0, x*y);
  screentexpos_cf_old = new unsigned char[x*y];
  memset(screentexpos_cf_old, 0, x*y);
  screentexpos_cbr_old = new unsigned char[x*y];
  memset(screentexpos_cbr_old, 0, x*y);

  gps.resize(x,y);
}

void renderer::swap_arrays() {
  screen = screen_old; screen_old = gps.screen; gps.screen = screen;
  screentexpos = screentexpos_old; screentexpos_old = gps.screentexpos; gps.screentexpos = screentexpos;
  screentexpos_addcolor = screentexpos_addcolor_old; screentexpos_addcolor_old = gps.screentexpos_addcolor; gps.screentexpos_addcolor = screentexpos_addcolor;
  screentexpos_grayscale = screentexpos_grayscale_old; screentexpos_grayscale_old = gps.screentexpos_grayscale; gps.screentexpos_grayscale = screentexpos_grayscale;
  screentexpos_cf = screentexpos_cf_old; screentexpos_cf_old = gps.screentexpos_cf; gps.screentexpos_cf = screentexpos_cf;
  screentexpos_cbr = screentexpos_cbr_old; screentexpos_cbr_old = gps.screentexpos_cbr; gps.screentexpos_cbr = screentexpos_cbr;

  gps.screen_limit = gps.screen + gps.dimx * gps.dimy * 4;
}

void enablerst::pause_async_loop()  {
  struct async_cmd cmd;
  cmd.cmd = async_cmd::pause;
  async_tobox.write(cmd);
  async_wait();
}

// Wait until the previous command has been acknowledged, /or/
// async_loop has quit. Incidentally execute any requests in the
// meantime.
void enablerst::async_wait() {
  if (loopvar == 0) return;
  async_msg r;
  bool reset_textures = false;
  for (;;) {
    async_frombox.read(r);
    switch (r.msg) {
    case async_msg::quit:
      loopvar = 0;
      return;
    case async_msg::complete:
      if (reset_textures) {
        puts("Resetting textures");
        textures.remove_uploaded_textures();
        textures.upload_textures();
      }
      return;
    case async_msg::set_fps:
      set_fps(r.fps);
      async_fromcomplete.write();
      break;
    case async_msg::set_gfps:
      set_gfps(r.fps);
      async_fromcomplete.write();
      break;
    case async_msg::push_resize:
      override_grid_size(r.x, r.y);
      async_fromcomplete.write();
      break;
    case async_msg::pop_resize:
      release_grid_size();
      async_fromcomplete.write();
      break;
    case async_msg::reset_textures:
      reset_textures = true;
      break;
    default:
      puts("EMERGENCY: Unknown case in async_wait");
      abort();
    }
  }
}

void enablerst::do_frame() {
  // Check how long it's been, exactly
  const Uint32 now = SDL_GetTicks();
  const Uint32 interval = CLAMP(now - last_tick, 0, 1000); // Anything above a second doesn't count
  // cout << last_tick << " + " << interval << " = " << now << endl;
  last_tick = now;

  // Update outstanding-frame counts
  outstanding_frames += interval * fps / 1000;
  outstanding_gframes += interval * gfps / 1000;
  if (outstanding_gframes > 3) {
    outstanding_gframes = 3;
  }
  // cout << outstanding_frames << " " << outstanding_gframes << endl;

  // Update the loop's tick-counter suitably
  if (outstanding_frames >= 1) {
    async_cmd cmd(async_cmd::inc);
    cmd.val = outstanding_frames;
    outstanding_frames -= cmd.val;
    async_tobox.write(cmd);
  }

  // Store the current time, for things that are fine with approximations
  enabler.clock = SDL_GetTicks();

  // If it's time to render..
  if (outstanding_gframes >= 1 &&
      (!sync || glClientWaitSync(sync, 0, 0) == GL_ALREADY_SIGNALED)) {
    // Get the async-loop to render_things
    async_cmd cmd(async_cmd::render);
    async_tobox.write(cmd);
    async_wait();
    // Then finish here
    renderer->display();
    renderer->render();
    gputicks.lock();
    gputicks.val++;
    gputicks.unlock();
    outstanding_gframes--;
  }

  // Sleep until the next gframe
  if (outstanding_gframes < 1) {
    float fragment = 1 - outstanding_gframes;
    float milliseconds = fragment / gfps * 1000;
    // cout << milliseconds << endl;
    SDL_Delay(milliseconds);
  }
}

void enablerst::override_grid_size(int x, int y) {
  if (SDL_ThreadID() != renderer_threadid) {
    // Ask the renderer to do it
    async_msg m(async_msg::push_resize);
    m.x = x; m.y = y;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    // We are the renderer; do it.
    overridden_grid_sizes.push(make_pair(init.display.grid_x,init.display.grid_y));
    renderer->grid_resize(x, y);
  }
}

void enablerst::release_grid_size() {
  if (SDL_ThreadID() != renderer_threadid) {
    async_frombox.write(async_msg(async_msg::pop_resize));
    async_fromcomplete.read();
  } else {
    if (!overridden_grid_sizes.size()) return;
    // FIXME: Find out whatever is causing release to be called too rarely; right now
    // we're overriding once per movie but apparently only releasing for the last one.
    pair<int,int> sz;
    while (overridden_grid_sizes.size()) {
      sz = overridden_grid_sizes.top();
      overridden_grid_sizes.pop();
    }
    zoom_display(zoom_resetgrid);
  }
}

void enablerst::zoom_display(zoom_commands command) {
  async_zoom.write(command);
}

int enablerst::calculate_fps() {
  if (frame_timings.size() < 50)
    return get_fps();
  else
    return calculated_fps;
}
int enablerst::calculate_gfps() {
  if (gframe_timings.size() < 50)
    return get_gfps();
  else
    return calculated_gfps;
}

void enablerst::do_update_fps(queue<int> &q, int &sum, int &last, int &calc) {
  while (q.size() > 50 && sum > 10000) {
    sum -= q.front();
    q.pop();
  }
  const int now = SDL_GetTicks();
  const int interval = now - last;
  q.push(interval);
  sum += interval;
  last = now;
  if (sum)
    calc = q.size() * 1000 / sum;
}

void enablerst::clear_fps() {
  while (frame_timings.size())
    frame_timings.pop();
  frame_sum = 0;
  frame_last = SDL_GetTicks();
  calculated_fps = get_fps();
}

void enablerst::update_fps() {
  do_update_fps(frame_timings, frame_sum, frame_last, calculated_fps);
}

void enablerst::update_gfps() {
  do_update_fps(gframe_timings, gframe_sum, gframe_last, calculated_gfps);
}

void enablerst::set_fps(int fps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_fps);
    m.fps = fps;
    async_paused = true;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (fps == 0)
      fps = 1048576;
    this->fps = fps;
    fps_per_gfps = fps / gfps;
    struct async_cmd cmd;
    cmd.cmd = async_cmd::set_fps;
    cmd.val = fps;
    async_tobox.write(cmd);
    async_tobox.write(async_cmd(async_cmd::start));
  }
}

void enablerst::set_gfps(int gfps) {
  if (SDL_ThreadID() != renderer_threadid) {
    async_msg m(async_msg::set_gfps);
    m.fps = gfps;
    async_frombox.write(m);
    async_fromcomplete.read();
  } else {
    if (gfps == 0)
      gfps = 50;
    this->gfps = gfps;
    fps_per_gfps = fps / gfps;
  }
}

char get_slot_and_addbit_uchar(unsigned char &addbit,long &slot,long checkflag,long slotnum)
{
  if(checkflag<0)return 0;

  //FIND PROPER SLOT
  slot=checkflag>>3;
  if(slot>=slotnum)return 0;

  //FIND PROPER BIT IN THAT SLOT
  addbit=1<<(checkflag%8);

  return 1;
}

void text_system_file_infost::initialize_info()
{
  std::ifstream fseed(filename.c_str());
  if(fseed.is_open())
    {
      string str;

      while(std::getline(fseed,str))
	{
	  if(str.length()>0)number++;
	}
    }
  else
    {
      string str;
      str="Error Initializing Text: ";
      str+=filename;
      errorlog_string(str);
    }
  fseed.close();
}

void text_system_file_infost::get_text(text_infost &text)
{
  text.clean();

  if(number==0)return;

  std::ifstream fseed(filename.c_str());
  if(fseed.is_open())
    {
      string str;

      int num=trandom(number);

      //SKIP AHEAD TO THE RIGHT SPOT
      while(num>0)
	{
	  std::getline(fseed,str);
	  num--;
	}

      //PROCESS THE STRING INTO TEXT ELEMENTS
      if(std::getline(fseed,str))
	{
	  int curpos;
	  string nextstr;
	  char doing_long=0;

	  text_info_elementst *newel;
	  long end=str.length();

	  while(end>0)
	    {
	      if(isspace(str[end-1]))end--;
	      else break;
	    }

	  str.resize(end);

	  for(curpos=0;curpos<end;curpos++)
	    {
	      //HANDLE TOKEN OR ENDING
	      //TWO FILE TOKENS IN A ROW MEANS LONG
	      //ONE MEANS STRING
	      if(str[curpos]==file_token || curpos==end-1)
		{
		  if(str[curpos]!=file_token)nextstr+=str[curpos];

		  //HAVE SOMETHING == SAVE IT
		  if(!nextstr.empty())
		    {
		      if(doing_long)
			{
			  newel=new text_info_element_longst(atoi(nextstr.c_str()));
			  text.element.push_back(newel);
			  doing_long=0;
			}
		      else
			{
			  newel=new text_info_element_stringst(nextstr);
			  text.element.push_back(newel);
			}

		      nextstr.erase();
		    }
		  //STARTING A LONG
		  else
		    {
		      doing_long=1;
		    }
		}
	      //JUST ADD IN ANYTHING ELSE
	      else
		{
		  nextstr+=str[curpos];
		}
	    }
	}
    }
  fseed.close();
}

void curses_text_boxst::add_paragraph(const string &src,int32_t para_width)
{
	stringvectst sp;
	sp.add_string(src);
	add_paragraph(sp,para_width);
}

void curses_text_boxst::add_paragraph(stringvectst &src,int32_t para_width)
{
	bool skip_leading_spaces=false;

	//ADD EACH OF THE STRINGS ON IN TURN
	string curstr;
	long strlength=0;
	long s,pos;
	for(s=0;s<src.str.size();s++)
		{
		//GRAB EACH WORD, AND SEE IF IT FITS, IF NOT START A NEW LINE
		for(pos=0;pos<src.str[s]->dat.size();pos++)
			{
			if(skip_leading_spaces)
				{
				if(src.str[s]->dat[pos]==' ')continue;
				else skip_leading_spaces=false;
				}

			//ADD TO WORD
			curstr+=src.str[s]->dat[pos];

			//IF TOO LONG, CUT BACK TO FIRST SPACE
			if(curstr.length()>para_width)
				{
				long opos=pos;

				long minus=0;
				do
					{
					pos--;
					minus++;
					}while(src.str[s]->dat[pos]!=' '&&pos>0);

				//IF WENT ALL THE WAY BACK, INTRODUCE A SPACE
				if(minus==curstr.size())
					{
					src.str[s]->dat.insert(opos-1," ");
					}
				else
					{
					curstr.resize(curstr.size()-minus);
					text.add_string(curstr);
					skip_leading_spaces=true;
					}
				curstr.erase();
				}
			}
		}

	//FLUSH FINAL BIT
	if(!curstr.empty())text.add_string(curstr);
}
