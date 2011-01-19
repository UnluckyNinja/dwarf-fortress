/**
 * @file
 * @date 12 juin 2010
 * @todo comment
 */

#ifndef INTERN_ENABLER_INPUT_HPP_
#define INTERN_ENABLER_INPUT_HPP_

#include "integer_types.hpp"
#include <string>
#include <set>
#include <list>

#include "meta/singleton.hpp"

#include "input/event.hpp"
#include "input/event_match.hpp"
#include "input/registered_key.hpp"
#include "input/key_event.hpp"
#include "input/mod_state.hpp"

#include "intern/events/event_engine.hpp"

class enabler_input: public singleton< enabler_input > {
  public:
    typedef ::std::set< ::std::string, less_sz > string_set_by_len_t;
    typedef ::std::list< interface_event > timeline_t;
    typedef ::std::set< event_match > event_match_set_t;
    typedef ::std::list< event_match > event_match_list_t;
    typedef ::std::map< ::std::string, interface_key_set_list_t > macros_t;
    typedef ::std::map< interface_key_t, repeat_speed > repeatmap_t;
    typedef ::std::map< interface_key_t, string_set_by_len_t > keydisplay_t;
    typedef ::std::multimap< event_match, interface_key_t > keymap_t;

    enabler_input();

    ::std::set< interface_key_t > key_translation(const event_match& match);

    /**
     * @brief returns the repeat value for the given interface key.
     */
    repeat_speed get_key_repeat(interface_key_t);
    /**
     * @brief sets the repeat value for the given interface key.
     */
    void set_key_repeat(interface_key_t, repeat_speed);

    /**
     * @brief loads macro definition from the given file.
     */
    void load_macro_from_file(const ::std::string &file);
    /**
     * @brief saves the give macro to the given file, under the given name.
     */
    void save_macro_to_file(const ::std::string &file, const ::std::string &name, const interface_key_set_list_t &);

    // In practice.. do not use this one.
    void add_input(sdl_event_t &e, uint32_t now);

    void on_key_event(const event< key_event > & event);
    void on_interface_event(const event< interface_event >& event);

    // Made specifically for curses. <0 = unicode, >0 = ncurses symbols.
#ifdef CURSES
    void add_input_ncurses(int key, uint32_t now, bool esc);
#endif
    interface_key_set_t get_input(::boost::system_time now = ::boost::get_system_time());
    void clear_input();

    void load_keybindings(const ::std::string &file);
    void save_keybindings(const ::std::string &file);
    void save_keybindings();
    ::std::string GetBindingTextDisplay(interface_key_t binding);

    // Macros
    // Records input until such a time as you say stop
    void record_input();
    // Stops recording, saving it as the active macro
    void record_stop();
    bool is_recording();
    // Runs the active macro, if any
    void play_macro();
    bool is_macro_playing();
    ::std::list< ::std::string > list_macros();
    // Loads some macro as the active one
    void load_macro(::std::string name);
    // Saves the active macro under some name
    void save_macro(::std::string name);
    void delete_macro(::std::string name);

    // Updating the key-bindings
    // Sets the next key-press to be stored instead of executed.
    void register_key();
    // Returns a description of stored keys. Max one of each type.
    ::std::list< registered_key > getRegisteredKey();
    // Binds one of the stored keys to key
    void bindRegisteredKey(match_type type, interface_key_t key);
    // Returns true if we're still waiting for a key-hit
    bool is_registering();

    // Returns a list of events matching this interfacekey
    ::std::list< event_match > list_keys(interface_key_t key);
    // Removes a particular matcher from the keymap.
    void remove_key(interface_key_t key, event_match ev);

    // These change dynamically in the normal process of DF
    // Input serial number, to differentiate distinct physical presses
    int32_t last_serial;
    int32_t next_serial();

    // A timeline of pending key events (for next get_input)
    ::boost::mutex timeline_mutex;
    timeline_t timeline;

    // Keys we consider "pressed"
    ::boost::mutex pressed_keys_mutex;
    event_match_set_t pressed_keys;

    // Modifier state
    mod_state state;
    mod_state& get_mod_state();

    uint16_t sdl_to_mod_state(const sdl_keyboard_event_t& e);

    // These do not change as part of the normal dynamics of DF, only at startup/when editing.
    keymap_t keymap;

    repeatmap_t repeatmap;

    // Used only for display, not for meaning
    keydisplay_t keydisplay;
    keydisplay_t& get_keydisplay();
    void update_keydisplay(interface_key_t binding, ::std::string display);

    // Macro recording
    bool macro_recording;
    // Active macro
    interface_key_set_list_t active_macro;
    macros_t macros;
    // Time at which the currently playing macro will end
    uint32_t macro_end;

    // Keybinding editing
    bool key_registering;
    event_match_list_t stored_keys;

    // Interface-file last loaded
    ::std::string interfacefile;
};

#endif /* INTERN_ENABLER_INPUT_HPP_ */
