/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef HIDDEN_DWARF_HPP_
#define HIDDEN_DWARF_HPP_

#include <set>

#include <string>
#include "integer_types.hpp"
#include "event_types.hpp"

struct textlinesst;

/**
 * @{
 * @ingroup hidden
 */
char beginroutine();
void endroutine();

char mainloop();

void dwarf_end_announcements();
void dwarf_option_screen();
void dwarf_remove_screen();

void drawborder(const char* str, char style = 0, const char* colorstr = 0);
void process_object_lines(textlinesst& lines, ::std::string& chktype, ::std::string& graphics_dir);

/**
 * @{
 * @brief standardscrolling function flags
 */
#define SCROLLING_NOSELECT (1 << 0)
#define SCROLLING_NO_WRAP  (1 << 1)
#define SCROLLING_REVERSE  (1 << 2)
/** @} */

char standardscrolling(::std::set< interface_key_t >& events, int32_t& selection, int32_t min, int32_t max,
    int32_t jump, uint32_t flag = 0);
/** @} */

#endif /* HIDDEN_DWARF_HPP_ */
