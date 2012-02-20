/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_SIGNALS_HPP_
#define DWARF_FORTRESS_SIGNALS_HPP_

namespace df {

  void add_kill_handler();
  bool kill_received();
  void kill();

} // namespace df

#endif /* DWARF_FORTRESS_SIGNALS_HPP_ */
