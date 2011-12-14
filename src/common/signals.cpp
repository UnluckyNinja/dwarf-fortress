/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include "signals.hpp"

#include <signal.h>

namespace df {

  static int _kill_received = 0;
  static void signal_handler(int signal_value) {
    _kill_received = 1;
  }

  void add_kill_handler() {
    struct sigaction action;
    action.sa_handler = signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, 0);
    sigaction(SIGTERM, &action, 0);
  }

  bool kill_received() {
    return _kill_received;
  }

} // namespace df
