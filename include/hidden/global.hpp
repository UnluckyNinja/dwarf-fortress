/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef HIDDEN_GLOBAL_HPP_
#define HIDDEN_GLOBAL_HPP_

#include <string>
#include "integer_types.hpp"

enum GameMode {
  GAMEMODE_DWARF, GAMEMODE_ADVENTURE, GAMEMODENUM, GAMEMODE_NONE
};

struct graphicst;
struct interfacest;
struct initst;
struct texture_handlerst;

/**
 * @ingroup hidden
 */
extern ::std::string errorlog_prefix;

/**
 * @ingroup hidden
 */
extern GameMode gamemode;
/**
 * @ingroup hidden
 */
extern int32_t movie_version;
/**
 * @ingroup hidden
 */
extern interfacest gview;
/**
 * @ingroup hidden
 */
extern initst init;
/**
 * @ingroup hidden
 */
extern texture_handlerst texture;

/**
 * @ingroup hidden
 */
extern int32_t basic_seed;
/**
 * @ingroup hidden
 */
extern short mt_cur_buffer;
/**
 * @ingroup hidden
 */
extern short mt_virtual_buffer;

#define MT_BUFFER_NUM 10
#define MT_LEN 624

/**
 * @ingroup hidden
 */
extern uint32_t mt_buffer[MT_BUFFER_NUM][MT_LEN];
/**
 * @ingroup hidden
 */
extern int mt_index[MT_BUFFER_NUM];

#endif /* HIDDEN_GLOBAL_HPP_ */
