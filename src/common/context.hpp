/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_DISPLAY_CONTEXT_HPP_
#define DWARF_FORTRESS_DISPLAY_CONTEXT_HPP_

namespace df {

  namespace display {

    void release();
    bool acquire();
    void swap_buffers();

  } // namespace display

} // namespace df

#endif /* DWARF_FORTRESS_DISPLAY_CONTEXT_HPP_ */
