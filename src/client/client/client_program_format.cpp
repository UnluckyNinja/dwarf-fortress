
/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 *
 */

#include "gtulu/namespaces.hpp"
#include "client_program_format.hpp"

namespace gtulu {
  namespace internal {
    namespace format {
      namespace program {

        // #template#<declare_shader_source/>

        char const* client_program_format::client_vertex_shader_shader_format::source = "#version 330 core\n"
"in vec2 position;\n"
"out vert {\n"
"  vec2 coordinates;\n"
"} vertex;\n"
"void main() {\n"
"  gl_Position = vec4(position, 0.0, 1.0);\n"
"  \n"
"  vertex.coordinates = (position + 1.0) / 2.0;\n"
"}\n"
;
        

        char const* client_program_format::client_fragment_shader_shader_format::source = "#version 330 core\n"
"in layout(origin_upper_left,pixel_center_integer) vec4 gl_FragCoord;\n"
"uniform ivec2 grid_size;\n"
"uniform ivec2 window_size;\n"
"// uniform uint last_frame;\n"
"// uniform usampler2D last_update;\n"
"uniform sampler2D characters;\n"
"uniform sampler2D texture_indexes;\n"
"uniform sampler2D texture_colors;\n"
"uniform sampler2DArray tilesets;\n"
"uniform usampler1D tilesets_info;\n"
"in vert {\n"
"  vec2 coordinates;\n"
"} vertex;\n"
"out vec4 output_image;\n"
"void main() {\n"
"  vec2 pos = vertex.coordinates;\n"
"  pos.y = 1 - pos.y;\n"
"  \n"
"  int max_grid = max(grid_size.x, grid_size.y);\n"
"  ivec3 grid_pos = ivec3(pos.x * max_grid, pos.y * max_grid, 0);\n"
"  vec3 grid_pos_off = vec3(pos.x * max_grid, pos.y * max_grid, 0.0f) - grid_pos;\n"
"  \n"
"  vec4 c = texelFetch(characters, grid_pos.yx, 0);\n"
"  vec4 tc = texelFetch(texture_colors, grid_pos.yx, 0);\n"
"  vec4 ti = texelFetch(texture_indexes, grid_pos.yx, 0);\n"
"  \n"
"  vec3 size = vec3(textureSize(tilesets, 0));\n"
"  \n"
"  uint char_id = 0u;\n"
"  uint char_index = uint(c.r * 0xFF);\n"
"  vec3 char_pos = vec3(char_index % 16u, char_index / 16u, char_id) + grid_pos_off;\n"
"  \n"
"  uvec2 char_info = texelFetch(tilesets_info, int(char_id), 0).xy;\n"
"  char_pos.x *= float(char_info.x);\n"
"  char_pos.y *= float(char_info.y);\n"
"  \n"
"  vec4 char_tile = texelFetch(tilesets, ivec3(char_pos), 0);\n"
"  \n"
"  uint tileset_id = uint(ti.r * 0xFF00) + uint(ti.g * 0xFF);\n"
"  vec3 tileset_pos = vec3(uint(ti.b * 0xFF), uint(ti.a * 0xFF), tileset_id) + grid_pos_off;\n"
"  \n"
"  uvec2 tileset_info = texelFetch(tilesets_info, int(tileset_id), 0).xy;\n"
"  tileset_pos.x *= tileset_info.x;\n"
"  tileset_pos.y *= tileset_info.y;\n"
"  \n"
"  tileset_pos.x /= size.x;\n"
"  tileset_pos.y /= size.y;\n"
"  tileset_pos.z /= size.z;\n"
"  \n"
"  vec4 tile = textureLod(tilesets, tileset_pos, 0);\n"
"  \n"
"  if(tile.r > 0.0 || tile.g > 0.0 || tile.b > 0.0 || tile.a > 0.0) {\n"
"    output_image = tile;\n"
"  } else {\n"
"    output_image = char_tile;\n"
"  }\n"
"  output_image.a = tile.r + tc.r;\n"
"}\n"
;
        
        ;

      } // namespace program
    } // namespace format
  } // namespace internal
} // namespace gtulu

