/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>
#include <SDL/SDL_syswm.h>

#include <boost/thread.hpp>

namespace df {
  namespace display {

    static Display* display;
    static GLXDrawable drawable;
    static GLXDrawable readable;
    static GLXContext context;
    static boost::mutex mutex;

    static int _x_error(Display *display, XErrorEvent *error) {
      return 0;
    }

    void release() {
      boost::lock_guard< boost::mutex > lock(mutex);

      SDL_SysWMinfo wm_info;
      SDL_VERSION(&wm_info.version);
      SDL_GetWMInfo(&wm_info);
      wm_info.info.x11.window;

      XSetErrorHandler(_x_error);

      display = wm_info.info.x11.gfxdisplay;

      int framebuffer_config_count = 0;
      GLXFBConfig* framebuffer_configs = NULL;
      framebuffer_configs = glXChooseFBConfig(display, DefaultScreen(display), NULL, &framebuffer_config_count);

      int context_attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3,
                                GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB, None };
      context = glXCreateContextAttribsARB(display, framebuffer_configs[0], NULL, true, context_attribs);
      XFree(framebuffer_configs);
      XSync(display, false);

      drawable = wm_info.info.x11.window;
      readable = wm_info.info.x11.window;

      glXMakeContextCurrent(display, 0, 0, NULL);

//      boost::lock_guard< boost::mutex > lock(mutex);
//
//      display = glXGetCurrentDisplay();
//      drawable = glXGetCurrentDrawable();
//      readable = glXGetCurrentReadDrawable();
//      context = glXGetCurrentContext();
//
//      glXMakeContextCurrent(display, 0, 0, NULL);
    }

    bool acquire() {
      boost::lock_guard< boost::mutex > lock(mutex);

      return glXMakeContextCurrent(display, drawable, readable, context);
    }

  } // namespace display
} // namespace df
