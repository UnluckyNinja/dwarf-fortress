/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_HPP_
#define INTERN_HPP_

/**
 * @defgroup intern Internal code
 * @brief Contains internal declarations and internal definitions of structures, methods and functions.
 * @details Internal declaration means that the structures, methods or functions declared will not be visible
 *   from outside libgraphics. Internal definition means that the structures, methods or functions will not be usable
 *   from outside libgraphics, but that their definition may be used by the dwarf binary, for some reasons, such as a
 *   structure that contains extern methods, called by the dwarf binary, and other methods that are intern only because
 *   not called from dwarf binary.
 */

#endif /* INTERN_HPP_ */
