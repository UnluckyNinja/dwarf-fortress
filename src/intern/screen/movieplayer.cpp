/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/screen/movieplayer.hpp"

#include "hidden/global.hpp"
#include "hidden/dwarf.hpp"
#include "extern/init.hpp"
#include "extern/music.hpp"
#include "extern/musicsound.hpp"
#include "extern/interface.hpp"
#include "extern/graphics.hpp"
#include "extern/enabler.hpp"

#include "extern/utils/strings.hpp"

#include "intern/utils/filesystem.hpp"

void viewscreen_movieplayerst::help() {
  if (is_playing)
    return;

  viewscreenst::help();
}

void viewscreen_movieplayerst::feed(::std::set< interface_key_t > &events) {
  if (events.count(INTERFACEKEY_LEAVESCREEN)) {
    events.clear();

    if (is_playing) {
      is_playing = 0;
      enabler.release_grid_size();
      if (gview.original_fps)
        enabler.set_fps(gview.original_fps);
      gview.supermovie_on = 0;
      gview.currentblocksize = 0;
      gview.nextfilepos = 0;
      gview.supermovie_pos = 0;
      maxmoviepos = 0;

#ifndef NO_FMOD
      musicsound.stop_sound();
#endif
    } else if (saving)
      saving = 0;
    else if (loading)
      loading = 0;
#ifdef DEBUG_MOVIE_EDIT
    else if(editing)editing=0;
#endif
    else {
      is_playing = 0;
      enabler.release_grid_size();
      if (gview.original_fps)
        enabler.set_fps(gview.original_fps);
      gview.supermovie_on = 0;
      gview.currentblocksize = 0;
      gview.nextfilepos = 0;
      gview.supermovie_pos = 0;
      maxmoviepos = 0;

      breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
      return;
    }
  } else if (saving) {
    standardstringentry(savename, 39, STRINGENTRY_LETTERS | STRINGENTRY_SPACE | STRINGENTRY_NUMBERS | STRINGENTRY_SYMBOLS, events);

    if (events.count(INTERFACEKEY_SELECT)) {
      ::std::string filename;
      filename = "data/movies/";
      filename += savename;
      filename += ".cmv";

      copy_file(gview.movie_file, filename);
      saving = 0;
    }
  } else if (loading) {
    if (events.count(INTERFACEKEY_SELECT)) {
      ::std::string filename;
      filename = "data/movies/";
      filename += filelist[selfile];

      if (filename != gview.movie_file) {
        copy_file(filename, gview.movie_file);
      }
      loading = 0;
    }

    standardscrolling(events, selfile, 0, filelist.size() - 1, 21);
  }
#ifdef DEBUG_MOVIE_EDIT
  else if(editing)
  {
    char entering=0;

    if(editing_menu)
    {
      if(enabler.mouse_lbut&&enabler.tracking_on)
      {
        int sx,sy;
        gps.get_mouse_text_coords(sx,sy);

        if(sx>=0&&sx<80&&sy>=0&&sy<25)
        {
          if(sx>=0&&sx<16&&sy>=0&&sy<16)
          {
            editing_char=sx+sy*16;
          }
          if(sx>=16&&sx<24&&sy>=0&&sy<16)
          {
            editing_screenf=sy%8;
            editing_screenb=sx-16;
            editing_screenbright=sy/8;
          }
          if(sx>=26&&sx<=51&&sy>=0&&sy<SOUND_CHANNELNUM)
          {
            editing_selected_sound=sy;
          }
          int frame=gview.supermovie_pos/4000;
          if(sx>=52&&sx<=78&&sy>=0&&sy<SOUND_CHANNELNUM)
          {
            gview.supermovie_sound_time[frame][sy]=editing_selected_sound;
          }
          if(sx==(init.display.grid_x-1)&&sy>=0&&sy<SOUND_CHANNELNUM)
          {
            gview.supermovie_sound_time[frame][sy]=-1;
          }
        }

        enabler.mouse_lbut=0;
      }

      if(enabler.mouse_rbut&&enabler.tracking_on)
      {
        editing_menu=0;
        enabler.mouse_rbut=0;
      }

      if(editing_selected_sound>=0&&editing_selected_sound<gview.supermovie_sound.str.size())
      {
        if(gview.c== '%')
        {
          delete gview.supermovie_sound.str[editing_selected_sound];
          gview.supermovie_sound.str.erase(editing_selected_sound);
        }
        else
        {
          standardstringentry(events,gview.supermovie_sound.str[editing_selected_sound]->dat,26,STRINGENTRY_LETTERS|STRINGENTRY_SPACE|STRINGENTRY_NUMBERS|STRINGENTRY_SYMBOLS);
          entering=1;
        }
      }
      else
      {
        if(gview.c== '#')gview.supermovie_sound.add_string("new_sound");
        if(gview.c== '+')gview.supermovie_delayrate++;
        if(gview.c== '-')gview.supermovie_delayrate--;
        if(gview.c== 'T')text_mode=1-text_mode;
      }
      if(gview.supermovie_delayrate<0)gview.supermovie_delayrate=0;
      if(gview.supermovie_delayrate>10)gview.supermovie_delayrate=10;
    }
    else
    {
      if(text_mode)
      {
        if(gview.c!=0)
        {
          int sx,sy;
          gps.get_mouse_text_coords(sx,sy);

          if(sx>=0&&sx<80&&sy>=0&&sy<25)
          {
            int curpos=gview.supermovie_pos+sy+sx*25;
            gview.supermoviebuffer[curpos]=gview.c;
            gview.supermoviebuffer[curpos+2000]=(editing_screenf&7)+((editing_screenb&7)<<3);
            if(editing_screenbright)gview.supermoviebuffer[curpos+2000]+=64;
          }
        }
      }
      else
      {
        if(gview.c== 'a')
        {
          int x2,y2;
          for(x2=0;x2<80;x2++)
          {
            for(y2=0;y2<25;y2++)
            {
              if(x2>0)
              {
                gview.supermoviebuffer[gview.supermovie_pos+y2+(x2-1)*25]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25];
                gview.supermoviebuffer[gview.supermovie_pos+y2+(x2-1)*25+2000]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25+2000];
              }
              if(x2==(init.display.grid_x-1))gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25]=0;
            }
          }
        }
        if(gview.c== 'd')
        {
          int x2,y2;
          for(x2=(init.display.grid_x-1);x2>=0;x2--)
          {
            for(y2=0;y2<(init.display.grid_y-1);y2++)
            {
              if(x2<(init.display.grid_x-1))
              {
                gview.supermoviebuffer[gview.supermovie_pos+y2+(x2+1)*25]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25];
                gview.supermoviebuffer[gview.supermovie_pos+y2+(x2+1)*25+2000]=gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25+2000];
              }
              if(x2==0)gview.supermoviebuffer[gview.supermovie_pos+y2+x2*25]=0;
            }
          }
        }
        if(gview.c== 'E')end_frame_pos=gview.supermovie_pos;
        if(gview.c== 'c')editing_copy_from=gview.supermovie_pos;
        if(gview.c== 'p')
        {
          int i;
          for(i=0;i<4000;i++)
          {
            gview.supermoviebuffer[gview.supermovie_pos+i]=gview.supermoviebuffer[editing_copy_from+i];
          }
        }
        if(gview.c== '+')gview.supermovie_pos+=4000;
        if(gview.c== '-')gview.supermovie_pos-=4000;
        if(gview.c== '/')gview.supermovie_pos-=40000;
        if(gview.c== '*')gview.supermovie_pos+=40000;
        if(gview.supermovie_pos<0)gview.supermovie_pos=0;
        if(gview.supermovie_pos>=MOVIEBUFFSIZE)gview.supermovie_pos=MOVIEBUFFSIZE-4000;
      }

      if(enabler.mouse_lbut&&enabler.tracking_on)
      {
        int sx,sy;
        gps.get_mouse_text_coords(sx,sy);

        if(sx>=0&&sx<80&&sy>=0&&sy<25)
        {
          int curpos=gview.supermovie_pos+sy+sx*25;
          gview.supermoviebuffer[curpos]=editing_char;
          gview.supermoviebuffer[curpos+2000]=(editing_screenf&7)+((editing_screenb&7)<<3);
          if(editing_screenbright)gview.supermoviebuffer[curpos+2000]+=64;
        }
      }
      if(enabler.mouse_rbut&&enabler.tracking_on)
      {
        editing_menu=1;
        enabler.mouse_rbut=0;
      }
    }

    if(!entering&&gview.c== 'S')
    {
      int opos=gview.supermovie_pos;
      gview.first_movie_write=1;
      gview.supermovie_pos=end_frame_pos+4000;

      gview.write_movie_chunk();

      gview.supermovie_pos=opos;
    }
  }
#endif
  else {
    if (is_playing) {
    } else {
#ifdef DEBUG_MOVIE_EDIT
      if(gview.c== 'E')
      {
        editing=1;
        gview.supermovie_pos=0;
      }
#endif

      if (events.count(INTERFACEKEY_MOVIE_RECORD)) {
        //TURN ON THE MOVIE RECORDER
        is_playing = 0;
        enabler.release_grid_size();
        if (gview.original_fps)
          enabler.set_fps(gview.original_fps);
        gview.supermovie_on = 1;
        gview.currentblocksize = 0;
        gview.nextfilepos = 0;
        gview.supermovie_pos = 0;
        gview.supermovie_delayrate = 0;
        gview.first_movie_write = 1;
        maxmoviepos = 0;

        breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
      }
      if (events.count(INTERFACEKEY_MOVIE_PLAY)) {
        is_playing = 1;
        gview.supermovie_on = 0;
        gview.currentblocksize = 0;
        gview.nextfilepos = 0;
        gview.supermovie_pos = 0;
        maxmoviepos = 0;
      }
      if (events.count(INTERFACEKEY_MOVIE_SAVE)) {
        savename.erase();
        saving = 1;
      }
      if (events.count(INTERFACEKEY_MOVIE_LOAD)) {
        selfile = 0;

        clearfilelist();

        ::std::vector< ::std::string > files;
        find_files_by_glob("data/movies/*.cmv", files);

        filelist = files;

        if (filelist.size() > 0)
          loading = 1;
      }
    }
  }
}

void viewscreen_movieplayerst::logic() {
  enabler.flag &= ~ENABLERFLAG_MAXFPS;

  enabler.flag |= ENABLERFLAG_RENDER;

  if (!force_file.empty() && !is_playing && !quit_if_no_play && is_forced_play) {
    is_playing = 1;
    quit_if_no_play = 1;
    gview.movie_file = force_file;
    gview.supermovie_on = 0;
    gview.currentblocksize = 0;
    gview.nextfilepos = 0;
    gview.supermovie_pos = 0;
    maxmoviepos = 0;
  }

  if (!is_playing && quit_if_no_play) {
    breakdownlevel = INTERFACE_BREAKDOWN_STOPSCREEN;
    return;
  }

  //LOAD A MOVIE BUFFER BY BUFFER
  if (is_playing) {
    if ((unsigned int) gview.supermovie_pos >= MOVIEBUFFSIZE || gview.currentblocksize == 0) {
      gview.read_movie_chunk(maxmoviepos, is_playing);
    }

    if (is_playing) {
      int half_frame_size = init.display.grid_x * init.display.grid_y;

#ifndef NO_FMOD
      //PLAY ANY RELEVANT SOUNDS
      if (gview.supermovie_delaystep == gview.supermovie_delayrate) {
        int fr = gview.supermovie_pos / (half_frame_size * 2);
        if (fr >= 0 && fr < 200) {
          int c, sd;
          for (c = 0; c < 16; c++) {
            sd = gview.supermovie_sound_time[fr][c];
            if (sd >= 0 && (unsigned int) sd < gview.supermovie_sound.str.size()) {
              musicsound.playsound(sd, c);
            }
          }
        }
      }
#endif

      //PRINT THE NEXT FRAME AND ADVANCE POSITION
      short x2, y2;
      for (x2 = 0; x2 < init.display.grid_x; x2++) {
        for (y2 = 0; y2 < init.display.grid_y; y2++) {
          gview.supermovie_pos++;
        }
      }
      if (gview.supermovie_delaystep == 0) {
        gview.supermovie_pos += half_frame_size;
        gview.supermovie_delaystep = gview.supermovie_delayrate;
      } else {
        gview.supermovie_pos -= half_frame_size;//RETURN TO LAST FRAME
        gview.supermovie_delaystep--;
      }

      //DONE
      if (gview.supermovie_pos >= maxmoviepos && (unsigned int) (maxmoviepos + half_frame_size * 2) < MOVIEBUFFSIZE) {
        is_playing = 0;
        //NOTE: THIS CAUSES IT TO LOSE THE LAST FRAME DUE TO CLEARING
      }
    }
  }
}

void viewscreen_movieplayerst::render() {
  if (breakdownlevel != INTERFACE_BREAKDOWN_NONE)
    return;

  if (!is_playing && is_forced_play)
    return;

  if (!quit_if_no_play) {
    if (editing)
      drawborder(NULL);
    else
      drawborder("  Moving Records  ");
  }

  //LOAD A MOVIE BUFFER BY BUFFER
  if (is_playing) {
    int half_frame_size = init.display.grid_x * init.display.grid_y;

    //PRINT THE NEXT FRAME AND ADVANCE POSITION
    drawborder(NULL, -1);

    short x2, y2;
    for (x2 = 0; x2 < init.display.grid_x; x2++) {
      for (y2 = 0; y2 < init.display.grid_y; y2++) {
        gps.locate(y2, x2);

        gps.changecolor((gview.supermoviebuffer[gview.supermovie_pos + half_frame_size] & 7),
                        (gview.supermoviebuffer[gview.supermovie_pos + half_frame_size] & 56) >> 3,
                        (gview.supermoviebuffer[gview.supermovie_pos + half_frame_size] & 64));

        gps.addchar(gview.supermoviebuffer[gview.supermovie_pos]);

        gview.supermovie_pos++;
      }
    }
    gview.supermovie_pos -= half_frame_size;//RETURN TO LAST FRAME
  } else if (loading) {
    ::std::size_t scroll = selfile / 21 * 21;
    ::std::size_t l;
    for (l = scroll; l < filelist.size() && l < scroll + 21; l++) {
      if (l == (unsigned int) selfile)
        gps.changecolor(7, 0, 1);
      else
        gps.changecolor(7, 0, 0);
      gps.locate(l - scroll + 2, 2);
      gps.addst(filelist[l]);
    }
  }
#ifdef DEBUG_MOVIE_EDIT
  else if(editing)
  {
    if(editing_menu)
    {
      int tx,ty;
      unsigned char c=0;
      for(ty=0;ty<16;ty++)
      {
        for(tx=0;tx<16;tx++)
        {
          gps.locate(ty,tx);
          gps.changecolor(editing_screenf,editing_screenb,editing_screenbright);
          gps.addchar(c);
          c++;
        }
      }
      gps.locate(18,0);
      gps.changecolor(editing_screenf,editing_screenb,editing_screenbright);
      gps.addchar(editing_char);
      for(ty=0;ty<16;ty++)
      {
        for(tx=0;tx<8;tx++)
        {
          gps.locate(ty,tx+16);
          gps.changecolor(ty%8,tx,ty/8);
          gps.addchar('A');
        }
      }

      gps.changecolor(7,0,1);
      gps.locate(20,0);
      string str;
      gps.addst("1/100 sec per frame: ");convert_long_to_string(gview.supermovie_delayrate,str);
      gps.addst(str);

      int scroll=(editing_selected_sound/25)*25;
      int e;
      for(e=scroll;e<scroll+25&&e<gview.supermovie_sound.str.size();e++)
      {
        if(e==editing_selected_sound)gps.changecolor(7,0,1);
        else gps.changecolor(7,0,0);
        gps.locate(e-scroll,26);
        gps.addst(gview.supermovie_sound.str[e]->dat);
      }

      int frame=gview.supermovie_pos/4000,sd;
      for(e=0;e<SOUND_CHANNELNUM;e++)
      {
        gps.changecolor(2,0,1);
        gps.locate(e-scroll,52);
        sd=gview.supermovie_sound_time[frame][e];
        if(sd>=0&&sd<gview.supermovie_sound.str.size())gps.addst(gview.supermovie_sound.str[sd]->dat);
        else
        {
          gps.addst("-----------------");
        }
        gps.changecolor(4,0,1);
        gps.locate(e-scroll,(init.display.grid_x-1));
        gps.addst("X");
      }
    }
    else
    {
      drawborder(NULL,-1);

      int curp=gview.supermovie_pos;
      int x2,y2;
      for(x2=0;x2<80;x2++)
      {
        for(y2=0;y2<25;y2++)
        {
          gps.locate(y2,x2);

          gps.changecolor((gview.supermoviebuffer[curp+2000] & 7),
              (gview.supermoviebuffer[curp+2000] & 56)>>3,
              (gview.supermoviebuffer[curp+2000] & 64));

          gps.addchar(gview.supermoviebuffer[curp]);

          curp++;
        }
      }

      if(enabler.mouse_y<150)gps.locate(24,0);
      else gps.locate(0,0);
      gps.changecolor(2,0,1);
      gps.addst("Frame: ");
      string num;
      convert_long_to_string(gview.supermovie_pos/4000+1,num);
      gps.addst(num);

      if(enabler.mouse_y<150)gps.locate(24,20);
      else gps.locate(0,20);
      gps.changecolor(3,0,1);
      gps.addst("Copy From: ");
      convert_long_to_string(editing_copy_from/4000+1,num);
      gps.addst(num);

      if(enabler.mouse_y<150)gps.locate(24,40);
      else gps.locate(0,40);
      gps.changecolor(4,0,1);
      gps.addst("Ends At: ");
      convert_long_to_string(end_frame_pos/4000+1,num);
      gps.addst(num);

      if(enabler.mouse_y<150)gps.locate(24,60);
      else gps.locate(0,60);
      int sx,sy;
      gps.get_mouse_text_coords(sx,sy);
      gps.changecolor(7,0,1);
      gps.addst("(");
      convert_long_to_string(sx,num);
      gps.addst(num);
      gps.addst(",");
      convert_long_to_string(sy,num);
      gps.addst(num);
      gps.addst(")");
    }
  }
#endif
  else {
    gps.changecolor(7, 0, 1);
    gps.locate(2, 2);
    gview.print_interface_token(INTERFACEKEY_MOVIE_RECORD);
    gps.addst(": Start recording (active record is erased, stops when you return here)");
    gps.locate(3, 2);
    gview.print_interface_token(INTERFACEKEY_MOVIE_PLAY);
    gps.addst(": Play the active moving record");
    gps.locate(4, 2);
    gview.print_interface_token(INTERFACEKEY_MOVIE_SAVE);
    gps.addst(": Save the active moving record (you will be prompted for a name)");
    gps.locate(5, 2);
    gview.print_interface_token(INTERFACEKEY_MOVIE_LOAD);
    gps.addst(": Load a moving record");

#ifdef DEBUG_MOVIE_EDIT
    gps.locate(7,2);
    gps.addst("E: Edit");
#endif

    if (saving) {
      gps.locate(10, 2);
      gps.addst("Name: ");
      gps.addst(savename);
    }
  }
}

char viewscreen_movieplayerst::movies_okay() {
  return 0;
}

void viewscreen_movieplayerst::clearfilelist() {
  for (::std::size_t f = 0; f < filelist.size(); f++)
    delete[] filelist[f];
  filelist.clear();
}

char viewscreen_movieplayerst::is_option_screen() {
  if (is_playing)
    return 2;
  else
    return 0;
}

viewscreen_movieplayerst::viewscreen_movieplayerst() {
  force_file.erase();
  gview.movie_file = "data/movies/last_record.cmv";
  is_playing = 0;
  enabler.release_grid_size();
  if (gview.original_fps)
    enabler.set_fps(gview.original_fps);
  is_forced_play = 0;
  quit_if_no_play = 0;
  gview.supermovie_on = 0;
  gview.currentblocksize = 0;
  gview.nextfilepos = 0;
  gview.supermovie_pos = 0;
  maxmoviepos = 0;
  saving = 0;
  loading = 0;
  editing = 0;
  text_mode = 0;
  editing_copy_from = 0;
  editing_char = 219;
  editing_screenf = 7;
  editing_screenb = 0;
  editing_screenbright = 0;
  editing_menu = 0;
  editing_selected_sound = 0;
  end_frame_pos = 0;
  gview.supermovie_sound.clean();
#ifndef NO_FMOD
  int i, c;
  for (i = 0; i < 200; i++) {
    for (c = 0; c < SOUND_CHANNELNUM; c++)
      gview.supermovie_sound_time[i][c] = -1;
  }
#endif
}

viewscreen_movieplayerst::~viewscreen_movieplayerst() {
  clearfilelist();
}

