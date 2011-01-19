/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "hidden/global.hpp"

#include "extern/texture_handler.hpp"
#include "extern/init.hpp"
#include "extern/interface.hpp"
#include "extern/graphics.hpp"

::std::string errorlog_prefix;

GameMode gamemode;
int32_t movie_version = 10001;

interfacest gview;
initst init;
texture_handlerst texture;

int32_t basic_seed;
short mt_cur_buffer;
short mt_virtual_buffer;
uint32_t mt_buffer[MT_BUFFER_NUM][MT_LEN];
int mt_index[MT_BUFFER_NUM];

