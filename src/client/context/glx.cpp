/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include <GL/glx.h>

#include <boost/thread.hpp>

namespace df {
  namespace display {

    static Display* display;
    static GLXDrawable drawable;
    static GLXDrawable readable;
    static GLXContext context;
    static boost::mutex mutex;

    void release() {
      boost::lock_guard< boost::mutex > lock(mutex);

      display = glXGetCurrentDisplay();
      drawable = glXGetCurrentDrawable();
      readable = glXGetCurrentReadDrawable();
      context = glXGetCurrentContext();

      glXMakeContextCurrent(display, 0, 0, NULL);
    }

    bool acquire() {
      boost::lock_guard< boost::mutex > lock(mutex);

      return glXMakeContextCurrent(display, drawable, readable, context);
    }

  } // namespace display
} // namespace df
