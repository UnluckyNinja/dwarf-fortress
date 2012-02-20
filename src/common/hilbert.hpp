/**
 * @file
 *
 * Copyright (c) 2008, Svein Ove Aas, parts by Tarn Adams, portions Bernard
 * Helyer, packaging and cleanup bits by Beren Minor. All rights reserved.
 *
 * See accompanying file README.rst
 */

#ifndef DWARF_FORTRESS_HILBERT_HPP_
#define DWARF_FORTRESS_HILBERT_HPP_

/**
 * Hilbert curve coordinate computation implementation using template
 * metaprogramming for pre-computed recursion.
 *
 * An Hilbert curve is a space filling curve which preserves locality, and
 * which allows us to transform 2D coordinates to integers.
 *
 * This implementation uses a maximum Hilbert depth which is by default 10,
 * which can map any (x,y) value with x and y < 2^10. This should be sufficient
 * for Dwarf Fortress.
 *
 * More information about Hilbert Curves on wikipedia:
 * http://en.wikipedia.org/wiki/Hilbert_curve
 */

#include <cstdint>
#include <map>

namespace df {

  namespace detail {

    /**
     * An Hilbert curve is a combination of a repeating pattern, and quadrant
     * subdivision.
     *
     * The base curve is the following, with four orientations:
     *
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *  | + | + |    | +---+ |    | +---+ |    | +---+ |
     *  +-|-+-|-+    +-|-+---+    +-|-+-|-+    +---+-|-+
     *  | +---+ |    | +---+ |    | + | + |    | +---+ |
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *     up          right        down         left
     *
     *  Each of these four patterns leads to different numbering and orientation
     *  for sub-quadrants. The rules are the following:
     *
     *  Numbering:
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *  | 0 | 3 |    | 2---3 |    | 2---1 |    | 0---1 |
     *  +-|-+-|-+    +-|-+---+    +-|-+-|-+    +---+-|-+
     *  | 1---2 |    | 1---0 |    | 3 | 0 |    | 3---2 |
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *     up          right        down         left
     *
     *  Orientation of sub-quadrants:
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *  | l | r |    | r---u |    | d---d |    | u---l |
     *  +-|-+-|-+    +-|-+---+    +-|-+-|-+    +---+-|-+
     *  | u---u |    | r---d |    | l | r |    | d---l |
     *  +---+---+    +---+---+    +---+---+    +---+---+
     *     up          right        down         left
     */

    namespace quadrant {
      struct upper_left;
      struct lower_left;
      struct upper_right;
      struct lower_right;
    } // namespace quadrant

    namespace orientation {
      struct up;
      struct down;
      struct left;
      struct right;
    } // namespace orientation

    template< typename Quadrant, typename Orientation >
    struct quadrant_info;

#define DECLARE_QUADRANT(quadrant_m, orientation_m, value_m, next_m)    \
  template< >                                                           \
  struct quadrant_info< quadrant::quadrant_m,                           \
                        orientation::orientation_m > {                  \
      static std::uint8_t const value = value_m;                        \
      typedef orientation::next_m next;                                 \
  };

    DECLARE_QUADRANT(upper_left, up, 0, left)
    DECLARE_QUADRANT(lower_left, up, 1, up)
    DECLARE_QUADRANT(lower_right, up, 2, up)
    DECLARE_QUADRANT(upper_right, up, 3, right)

    DECLARE_QUADRANT(upper_left, right, 2, right)
    DECLARE_QUADRANT(lower_left, right, 1, right)
    DECLARE_QUADRANT(lower_right, right, 0, down)
    DECLARE_QUADRANT(upper_right, right, 3, up)

    DECLARE_QUADRANT(upper_left, down, 2, down)
    DECLARE_QUADRANT(lower_left, down, 3, left)
    DECLARE_QUADRANT(lower_right, down, 0, right)
    DECLARE_QUADRANT(upper_right, down, 1, down)

    DECLARE_QUADRANT(upper_left, left, 0, up)
    DECLARE_QUADRANT(lower_left, left, 3, down)
    DECLARE_QUADRANT(lower_right, left, 2, left)
    DECLARE_QUADRANT(upper_right, left, 1, left)

#undef DECLARE_QUADRANT

    template< std::size_t HilbertDepth = 10, typename Orientation = orientation::up >
    struct hilbert_map;

    /* Base case to stop template specialization recursion */
    template< typename Orientation >
    struct hilbert_map< 0, Orientation > {
        typedef quadrant_info< quadrant::upper_left, Orientation > upper_left_info_t;
        typedef quadrant_info< quadrant::lower_left, Orientation > lower_left_info_t;
        typedef quadrant_info< quadrant::lower_right, Orientation > lower_right_info_t;
        typedef quadrant_info< quadrant::upper_right, Orientation > upper_right_info_t;

        static inline std::uint64_t map(std::uint32_t const x, std::uint32_t const y) {
          if (x & 1) {
            if (y & 1) {
              return lower_right_info_t::value;
            } else {
              return upper_right_info_t::value;
            }
          } else {
            if (y & 1) {
              return lower_left_info_t::value;
            } else {
              return upper_left_info_t::value;
            }
          }
        }

        static inline void unmap(std::uint64_t const value, std::uint32_t& x, std::uint32_t& y) {
          std::uint8_t const match = value & 0b11;

          if (match == lower_right_info_t::value) {
            x = 1;
            y = 1;
          } else if (match == upper_right_info_t::value) {
            x = 1;
            y = 0;
          } else if (match == lower_left_info_t::value) {
            x = 0;
            y = 1;
          } else if (match == upper_left_info_t::value) {
            x = 0;
            y = 0;
          }
        }
    };

    template< std::size_t HilbertDepth, typename Orientation >
    struct hilbert_map {
        typedef quadrant_info< quadrant::upper_left, Orientation > upper_left_info_t;
        typedef quadrant_info< quadrant::lower_left, Orientation > lower_left_info_t;
        typedef quadrant_info< quadrant::lower_right, Orientation > lower_right_info_t;
        typedef quadrant_info< quadrant::upper_right, Orientation > upper_right_info_t;

        typedef hilbert_map< HilbertDepth - 1, typename upper_left_info_t::next > upper_left_t;
        typedef hilbert_map< HilbertDepth - 1, typename lower_left_info_t::next > lower_left_t;
        typedef hilbert_map< HilbertDepth - 1, typename lower_right_info_t::next > lower_right_t;
        typedef hilbert_map< HilbertDepth - 1, typename upper_right_info_t::next > upper_right_t;

        static std::uint64_t const hilbert_shift = 2 * HilbertDepth;
        static std::uint64_t const hilbert_match = 1 << HilbertDepth;

        static inline std::uint64_t map(std::uint32_t const x, std::uint32_t const y) {
          if (x & hilbert_match) {
            if (y & hilbert_match) {
              return (lower_right_info_t::value << hilbert_shift) | lower_right_t::map(x, y);
            } else {
              return (upper_right_info_t::value << hilbert_shift) | upper_right_t::map(x, y);
            }
          } else {
            if (y & hilbert_match) {
              return (lower_left_info_t::value << hilbert_shift) | lower_left_t::map(x, y);
            } else {
              return (upper_left_info_t::value << hilbert_shift) | upper_left_t::map(x, y);
            }
          }
        }

        static inline void unmap(std::uint64_t const value, std::uint32_t& x, std::uint32_t& y) {
          std::uint8_t const match = (value >> hilbert_shift) & 0b11;

          if (match == lower_right_info_t::value) {
            lower_right_t::unmap(value, x, y);
            x |= hilbert_match;
            y |= hilbert_match;
          } else if (match == upper_right_info_t::value) {
            upper_right_t::unmap(value, x, y);
            x |= hilbert_match;
            y &= ~hilbert_match;
          } else if (match == lower_left_info_t::value) {
            lower_left_t::unmap(value, x, y);
            x &= ~hilbert_match;
            y |= hilbert_match;
          } else if (match == upper_left_info_t::value) {
            upper_left_t::unmap(value, x, y);
            x &= ~hilbert_match;
            y &= ~hilbert_match;
          }
        }
    };

  } // namespace detail

  template< std::size_t HilbertDepth = 10 >
  static inline std::uint64_t to_hilbert(std::uint32_t const x, std::uint32_t const y) {
    return detail::hilbert_map< HilbertDepth >::map(x, y);
  }
  template< std::size_t HilbertDepth = 10 >
  static inline void to_hilbert(std::uint64_t& hilbert, std::uint32_t const x, std::uint32_t const y) {
    hilbert = detail::hilbert_map< HilbertDepth >::map(x, y);
  }
  template< std::size_t HilbertDepth = 10 >
  static inline std::pair< std::uint32_t, std::uint32_t > from_hilbert(std::uint64_t const value) {
    std::uint32_t x = 0;
    std::uint32_t y = 0;
    detail::hilbert_map< HilbertDepth >::unmap(value, x, y);

    return std::make_pair(x, y);
  }
  template< std::size_t HilbertDepth = 10 >
  static inline void from_hilbert(std::uint64_t const value, std::uint32_t& x, std::uint32_t& y) {
    detail::hilbert_map< HilbertDepth >::unmap(value, x, y);
  }

  template< std::size_t HilbertDepth = 10 >
  static inline std::vector< std::uint64_t > to_hilbert_range(std::uint32_t const from_x,
                                                              std::uint32_t const from_y,
                                                              std::uint32_t const to_x,
                                                              std::uint32_t const to_y) {
    std::uint64_t hilbert_start = df::to_hilbert(from_x, from_y);
    std::uint64_t hilbert_end = df::to_hilbert(to_x, to_y);

    for (std::uint32_t x = from_x; x < to_x; ++x) {
      {
        std::uint64_t const hilbert = df::to_hilbert(x, from_y);
        hilbert_start = std::min(hilbert_start, hilbert);
        hilbert_end = std::max(hilbert_end, hilbert);
      }

      {
        std::uint64_t const hilbert = df::to_hilbert(x, to_y);
        hilbert_start = std::min(hilbert_start, hilbert);
        hilbert_end = std::max(hilbert_end, hilbert);
      }
    }

    for (std::uint32_t y = from_y; y < to_y; ++y) {
      {
        std::uint64_t const hilbert = df::to_hilbert(from_x, y);
        hilbert_start = std::min(hilbert_start, hilbert);
        hilbert_end = std::max(hilbert_end, hilbert);
      }

      {
        std::uint64_t const hilbert = df::to_hilbert(to_x, y);
        hilbert_start = std::min(hilbert_start, hilbert);
        hilbert_end = std::max(hilbert_end, hilbert);
      }
    }

    std::vector< std::uint64_t > hilbert_range;
    for (std::uint64_t h = hilbert_start; h < hilbert_end; ++h) {
      std::pair< std::uint32_t, std::uint32_t > const coord = df::from_hilbert(h);
      std::uint32_t const x = coord.first;
      std::uint32_t const y = coord.second;

      if (x >= from_x && x < to_x && y >= from_y && y < to_y) {
        hilbert_range.push_back(h);
      }
    }

    return hilbert_range;
  }

} // namespace df

#endif /* DWARF_FORTRESS_HILBERT_HPP_ */
