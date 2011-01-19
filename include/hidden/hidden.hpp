/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef HIDDEN_HPP_
#define HIDDEN_HPP_

/**
 * @defgroup hidden Hidden code
 * @brief Contains declarations of structures, methods and functions that are actually compiled in dwarf binary.
 * @details Hidden declaration means that the structures, methods or functions declared will be visible
 *   from libgraphics, but are actually embedded in dwarf binary and, first, should not be redefined, and second,
 *   should not be modified at all. There are hidden definitions of every declaration for testing purpose, in the src/hidden
 *   folder, that are compiled outside libgraphics, in the tetris binary, but those definitions do not correspond to the real
 *   ones, in the dwarf binary.
 */

#endif /* HIDDEN_HPP_ */
