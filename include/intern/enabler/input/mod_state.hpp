/**
 * @file
 * @date 25 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_MOD_STATE_HPP_
#define INTERN_ENABLER_INPUT_MOD_STATE_HPP_

#include "integer_types.hpp"
#include "sdl_types.hpp"
#include <iosfwd>

struct mod_state {
    enum flags {
      MOD_NONE = 0,
      MOD_SHIFT = 1 << 0,
      MOD_CTRL = 1 << 1,
      MOD_ALT = 1 << 2,
      MOD_META = 1 << 3,
      MOD_SUPER = 1 << 4,
      MOD_MODE = 1 << 5,
      MOD_COMPOSE = 1 << 6
    };

    typedef uint8_t value_t;

  private:
    value_t value;

  public:
    mod_state();
    mod_state(const value_t value);

    void clear();

    bool update(const sdl_keyboard_event_t&);
    bool update(const mod_state& mod_state_in);

    operator uint32_t() const;

    friend ::std::ostream& operator<<(::std::ostream&, const mod_state&);
};

::std::ostream& operator<<(::std::ostream&, const mod_state&);

#endif /* INTERN_ENABLER_INPUT_MOD_STATE_HPP_ */
