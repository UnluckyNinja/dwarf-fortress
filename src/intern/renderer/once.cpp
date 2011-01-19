/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/renderer/once.hpp"

void renderer_once::update_tile(int x, int y) {
  write_tile_vertexes(x, y, vertexes + tile_count * 6 * 2);
  write_tile_arrays(x, y, fg + tile_count * 6 * 4, bg + tile_count * 6 * 4, tex + tile_count * 6 * 2);
  tile_count++;
}

void renderer_once::draw(int dummy) {
  renderer_opengl::draw(tile_count * 6);
  tile_count = 0;
}

renderer_once::renderer_once() {
  tile_count = 0;
}
