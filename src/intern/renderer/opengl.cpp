/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/utils/opengl.hpp"

#include "intern/renderer/opengl.hpp"

#include "hidden/global.hpp"
#include "extern/init.hpp"
#include "extern/graphics.hpp"

#include "intern/texture/full_id.hpp"

#include "intern/utils/numbers.hpp"

#include <SDL_image.h>

bool renderer_opengl::uses_opengl() {
  return true;
}

bool renderer_opengl::init_video(int w, int h) {
  // Get ourselves an opengl-enabled SDL window
  uint32_t flags = SDL_HWSURFACE | SDL_OPENGL;

  // Set it up for windowed or fullscreen, depending.
  if (enabler.is_fullscreen()) {
    flags |= SDL_FULLSCREEN;
  } else {
    if (!init.display.flag.has_flag(INIT_DISPLAY_FLAG_NOT_RESIZABLE))
      flags |= SDL_RESIZABLE;
  }

  // Setup OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, init.window.flag.has_flag(INIT_WINDOW_FLAG_VSYNC_ON));
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER) ? 0 : 1);

  // (Re)create the window
  screen = SDL_SetVideoMode(w, h, 32, flags);

  if (!screen)
    return false;

  // Test double-buffering status
  int test;
  SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &test);
  if (test != ((init.display.flag.has_flag(INIT_DISPLAY_FLAG_SINGLE_BUFFER)) ? 0 : 1)) {
    if (enabler.is_fullscreen())
      ;
      //errorlog << "Requested single-buffering not available\n" << flush;
else
      __errorM(opengl) << "Requested single-buffering not available";
    }

    // (Re)initialize GLEW. Technically only needs to be done once on
    // linux, but on windows forgetting will cause crashes.
    glewInit();

    // Set the viewport and clear
    glViewport(0, 0, screen->w, screen->h);
    glClear(GL_COLOR_BUFFER_BIT);

    return true;
  }

void renderer_opengl::write_tile_vertexes(GLfloat x, GLfloat y, GLfloat *vertex) {
  vertex[0] = x; // Upper left
  vertex[1] = y;
  vertex[2] = x + 1; // Upper right
  vertex[3] = y;
  vertex[4] = x; // Lower left
  vertex[5] = y + 1;
  vertex[6] = x; // Lower left again (triangle 2)
  vertex[7] = y + 1;
  vertex[8] = x + 1; // Upper right
  vertex[9] = y;
  vertex[10] = x + 1; // Lower right
  vertex[11] = y + 1;
}

void renderer_opengl::allocate(int tiles) {
  vertexes = static_cast< GLfloat* > (realloc(vertexes, sizeof(GLfloat) * tiles * 2 * 6));
  assert(vertexes);
  fg = static_cast< GLfloat* > (realloc(fg, sizeof(GLfloat) * tiles * 4 * 6));
  assert(fg);
  bg = static_cast< GLfloat* > (realloc(bg, sizeof(GLfloat) * tiles * 4 * 6));
  assert(bg);
  tex = static_cast< GLfloat* > (realloc(tex, sizeof(GLfloat) * tiles * 2 * 6));
  assert(tex);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vertexes);
}

void renderer_opengl::init_opengl() {
  enabler.textures.upload_textures();
}

void renderer_opengl::uninit_opengl() {
  enabler.textures.remove_uploaded_textures();
}

void renderer_opengl::draw(int vertex_count) {
  // Render the background colors
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  glColorPointer(4, GL_FLOAT, 0, bg);
  glDrawArrays(GL_TRIANGLES, 0, vertex_count);
  // Render the foreground, colors and textures both
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexCoordPointer(2, GL_FLOAT, 0, tex);
  glColorPointer(4, GL_FLOAT, 0, fg);
  glDrawArrays(GL_TRIANGLES, 0, vertex_count);

  __gl_check_errors;
}

void renderer_opengl::write_tile_arrays(int x, int y, GLfloat *fg, GLfloat *bg, GLfloat *tex) {
  Either< texture_fullid, texture_ttfid > id = screen_to_texid(x, y);
  if (id.isL) { // An ordinary tile
    const gl_texpos *txt = enabler.textures.gl_texpos;
    // TODO: Only bother to set the one that's actually read in flat-shading mode
    // And set flat-shading mode.
    for (int i = 0; i < 6; i++) {
      *(fg++) = id.left.r;
      *(fg++) = id.left.g;
      *(fg++) = id.left.b;
      *(fg++) = 1;

      *(bg++) = id.left.br;
      *(bg++) = id.left.bg;
      *(bg++) = id.left.bb;
      *(bg++) = 1;
    }
    // Set texture coordinates
    *(tex++) = txt[id.left.texpos].left; // Upper left
    *(tex++) = txt[id.left.texpos].bottom;
    *(tex++) = txt[id.left.texpos].right; // Upper right
    *(tex++) = txt[id.left.texpos].bottom;
    *(tex++) = txt[id.left.texpos].left; // Lower left
    *(tex++) = txt[id.left.texpos].top;

    *(tex++) = txt[id.left.texpos].left; // Lower left
    *(tex++) = txt[id.left.texpos].top;
    *(tex++) = txt[id.left.texpos].right; // Upper right
    *(tex++) = txt[id.left.texpos].bottom;
    *(tex++) = txt[id.left.texpos].right; // Lower right
    *(tex++) = txt[id.left.texpos].top;
  } else {
    // TODO
  }
}

void renderer_opengl::update_tile(int x, int y) {
  const int tile = x * gps.dimy + y;
  // Update the arrays
  GLfloat *fg = this->fg + tile * 4 * 6;
  GLfloat *bg = this->bg + tile * 4 * 6;
  GLfloat *tex = this->tex + tile * 2 * 6;
  write_tile_arrays(x, y, fg, bg, tex);
}

void renderer_opengl::update_all() {
  glClear(GL_COLOR_BUFFER_BIT);
  for (int x = 0; x < gps.dimx; x++)
    for (int y = 0; y < gps.dimy; y++)
      update_tile(x, y);
}

void renderer_opengl::render() {
  draw(gps.dimx * gps.dimy * 6);
  if (init.display.flag.has_flag(INIT_DISPLAY_FLAG_ARB_SYNC) && GL_ARB_sync) {
    assert(enabler.sync == NULL);
    enabler.sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }
  SDL_GL_SwapBuffers();
}

renderer_opengl::renderer_opengl() {
  // Init member variables so realloc'll work
  screen = NULL;
  vertexes = NULL;
  fg = NULL;
  bg = NULL;
  tex = NULL;
  zoom_steps = forced_steps = 0;

  // Disable key repeat
  SDL_EnableKeyRepeat(0, 0);
  // Set window title/icon.
  SDL_WM_SetCaption(GAME_TITLE_STRING, NULL);
  SDL_Surface *icon = IMG_Load("data/art/icon.png");
  if (icon != NULL) {
    SDL_WM_SetIcon(icon, NULL);
    // The icon's surface doesn't get used past this point.
    SDL_FreeSurface(icon);
  }

  // Find the current desktop resolution if fullscreen resolution is auto
  if (init.display.desired_fullscreen_width == 0 || init.display.desired_fullscreen_height == 0) {
    const struct SDL_VideoInfo *info = SDL_GetVideoInfo();
    init.display.desired_fullscreen_width = info->current_w;
    init.display.desired_fullscreen_height = info->current_h;
  }

  // Initialize our window
  bool worked = init_video(enabler.is_fullscreen() ? init.display.desired_fullscreen_width : init.display.desired_windowed_width,
                           enabler.is_fullscreen() ? init.display.desired_fullscreen_height : init.display.desired_windowed_height);

  // Fallback to windowed mode if fullscreen fails
  if (!worked && enabler.is_fullscreen()) {
    enabler.fullscreen = false;
    __errorM(sdl)
      << "SDL initialization failure, trying windowed mode " << SDL_GetError();
    worked = init_video(init.display.desired_windowed_width, init.display.desired_windowed_height);
  }
  // Quit if windowed fails
  if (!worked) {
    __errorM(sdl)
      << "SDL initialization failure" << SDL_GetError();
    exit(EXIT_FAILURE);
  }

  // Initialize opengl
  init_opengl();
}

void renderer_opengl::zoom(zoom_commands cmd) {
  ::std::pair< int, int > before = compute_zoom(true);
  int before_steps = zoom_steps;
  switch (cmd) {
    case zoom_in:
      zoom_steps -= init.input.zoom_speed;
      break;
    case zoom_out:
      zoom_steps += init.input.zoom_speed;
      break;
    case zoom_reset:
      zoom_steps = 0;
    case zoom_resetgrid:
      compute_forced_zoom();
      break;
    default:
      break;
  }
  ::std::pair< int, int > after = compute_zoom(true);
  if (after == before && (cmd == zoom_in || cmd == zoom_out))
    zoom_steps = before_steps;
  else
    reshape(after);
}

void renderer_opengl::compute_forced_zoom() {
  forced_steps = 0;
  ::std::pair< int, int > zoomed = compute_zoom();
  while (zoomed.first < MIN_GRID_X || zoomed.second < MIN_GRID_Y) {
    forced_steps++;
    zoomed = compute_zoom();
  }
  while (zoomed.first > MAX_GRID_X || zoomed.second > MAX_GRID_Y) {
    forced_steps--;
    zoomed = compute_zoom();
  }
}

::std::pair< int, int > renderer_opengl::compute_zoom(bool clamp) {
  const int dispx = enabler.is_fullscreen() ? init.font.large_font_dispx : init.font.small_font_dispx;
  const int dispy = enabler.is_fullscreen() ? init.font.large_font_dispy : init.font.small_font_dispy;
  int w, h;
  if (dispx < dispy) {
    w = natural_w + zoom_steps + forced_steps;
    h = double(natural_h) * (double(w) / double(natural_w));
  } else {
    h = natural_h + zoom_steps + forced_steps;
    w = double(natural_w) * (double(h) / double(natural_h));
  }
  if (clamp) {
    w = ::numbers::clamp(w, MIN_GRID_X, MAX_GRID_X);
    h = ::numbers::clamp(h, MIN_GRID_Y, MAX_GRID_Y);
  }
  return ::std::make_pair(w, h);
}

// Parameters: grid units
void renderer_opengl::reshape(::std::pair< int, int > size) {
  int w = ::numbers::clamp(size.first, MIN_GRID_X, MAX_GRID_X);
  int h = ::numbers::clamp(size.second, MIN_GRID_Y, MAX_GRID_Y);
#ifdef DEBUG
  __info
  << "Resizing grid to " << w << "x" << h;
#endif
  gps_allocate(w, h);
  reshape_gl();
}

bool renderer_opengl::get_mouse_coords(int &x, int &y) {
  int mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  mouse_x -= off_x;
  mouse_y -= off_y;
  if (mouse_x < 0 || mouse_y < 0 || mouse_x >= size_x || mouse_y >= size_y)
    return false; // Out of bounds
  x = double(mouse_x) / double(size_x) * double(gps.dimx);
  y = double(mouse_y) / double(size_y) * double(gps.dimy);
  return true;
}

void renderer_opengl::reshape_gl() {
  // Allocate array memory
  allocate(gps.dimx * gps.dimy);
  // Initialize the vertex array
  int tile = 0;
  for (GLfloat x = 0; x < gps.dimx; x++)
    for (GLfloat y = 0; y < gps.dimy; y++, tile++)
      write_tile_vertexes(x, y, vertexes + 6 * 2 * tile);
  // Setup invariant state
  glEnableClientState(GL_COLOR_ARRAY);
  /// Set up our coordinate system
  if (forced_steps + zoom_steps == 0 && init.display.flag.has_flag(INIT_DISPLAY_FLAG_BLACK_SPACE)) {
    size_x = gps.dimx * dispx;
    size_y = gps.dimy * dispy;
    off_x = (screen->w - size_x) / 2;
    off_y = (screen->h - size_y) / 2;
  } else {
    // If we're zooming (or just not using black space), we use the
    // entire window.
    size_x = screen->w;
    size_y = screen->h;
    off_x = off_y = 0;
  }
  glViewport(off_x, off_y, size_x, size_y);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, gps.dimx, gps.dimy, 0);
}

// Parameters: window size
void renderer_opengl::resize(int w, int h) {
  // (Re)calculate grid-size
  dispx = enabler.is_fullscreen() ? init.font.large_font_dispx : init.font.small_font_dispx;
  dispy = enabler.is_fullscreen() ? init.font.large_font_dispy : init.font.small_font_dispy;
  natural_w = ::std::max(w / dispx, 1);
  natural_h = ::std::max(h / dispy, 1);
  // Compute forced_steps so we satisfy our grid-size limits
  compute_forced_zoom();
  // Force a full display cycle
  gps.force_full_display_count = 1;
  enabler.flag |= ENABLERFLAG_RENDER;
  // Reinitialize the video
  uninit_opengl();
  init_video(w, h);
  init_opengl();
  // Only reshape if we're free to pick grid size
  if (enabler.overridden_grid_sizes.size() == 0)
    reshape(compute_zoom());
}

// Parameters: grid size
void renderer_opengl::grid_resize(int w, int h) {
  reshape(::std::make_pair(w, h));
}

void renderer_opengl::set_fullscreen() {
  if (enabler.is_fullscreen()) {
    init.display.desired_windowed_width = screen->w;
    init.display.desired_windowed_height = screen->h;
    resize(init.display.desired_fullscreen_width, init.display.desired_fullscreen_height);
  } else {
    resize(init.display.desired_windowed_width, init.display.desired_windowed_height);
  }
}
