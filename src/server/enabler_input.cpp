/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#include <set>

#include "g_src/enabler.h"
#include "g_src/init.h"
#include "g_src/enabler_input.h"

std::set< InterfaceKey > inputs;

std::set< InterfaceKey > enabler_inputst::get_input(Time now) {
  std::set< InterfaceKey > out;
  inputs.swap(out);
  return out;
}
