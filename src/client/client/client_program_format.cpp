
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
"uniform uint width;\n"
"uniform uint height;\n"
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
"uniform uint width;\n"
"uniform uint height;\n"
"uniform float last_frame;\n"
"uniform sampler2D last_update;\n"
"uniform sampler2D characters;\n"
"uniform sampler2D tiles;\n"
"uniform sampler2D tiles_color;\n"
"uniform bool use_tile;\n"
"uniform uvec2 tile_size;\n"
"uniform sampler3D tilesets;\n"
"in vert {\n"
"  vec2 coordinates;\n"
"} vertex;\n"
"out vec4 output_image;\n"
"void main() {\n"
"  vec2 size = textureSize(characters, 0);\n"
"  vec2 pos = vertex.coordinates;\n"
"  pos.x = pos.x * width;\n"
"  pos.y = (1 - pos.y) * height;\n"
"  ivec2 ipos = ivec2(pos.y, pos.x);\n"
"  \n"
"  vec4 c = texelFetch(characters, ipos, 0);\n"
"  if (use_tile) {\n"
"    vec4 t = texture(tiles, pos);\n"
"    vec4 t_color = texture(tiles_color, pos);\n"
"    vec4 o = texture(tilesets, vec3(pos, t.r));\n"
"    o.r += t_color.r;\n"
"    o.g += t_color.g;\n"
"    o.b += t_color.b;\n"
"    output_image = o;\n"
"  } else {\n"
"    output_image = c;\n"
"  }\n"
"  vec4 last = texelFetch(last_update, ipos, 0);\n"
"  output_image.a = (last.r / last_frame) * (last.r / last_frame);\n"
"}\n"
;
        
        ;

      } // namespace program
    } // namespace format
  } // namespace internal
} // namespace gtulu

