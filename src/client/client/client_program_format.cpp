
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
"in layout(origin_upper_left,pixel_center_integer) vec4 gl_FragCoord;\n"
"uniform uint width;\n"
"uniform uint height;\n"
"uniform uint last_frame;\n"
"uniform usampler2D last_update;\n"
"uniform sampler2D characters;\n"
"uniform sampler2D texture_colors;\n"
"uniform sampler2D texture_indexes;\n"
"in vert {\n"
"  vec2 coordinates;\n"
"} vertex;\n"
"out vec4 output_image;\n"
"void main() {\n"
"  vec2 size = textureSize(characters, 0);\n"
"  vec2 pos = vertex.coordinates;\n"
"  uint maxsize = max(width, height);\n"
"  pos.x = pos.x * maxsize;\n"
"  pos.y = (1 - pos.y) * maxsize;\n"
"  ivec2 ipos = ivec2(pos.y, pos.x);\n"
"  \n"
"  vec4 c = texelFetch(characters, ipos, 0);\n"
"  vec4 tc = texelFetch(texture_colors, ipos, 0);\n"
"  vec4 ti = texelFetch(texture_indexes, ipos, 0);\n"
"  output_image = c + tc + ti;\n"
"  uvec4 last = texelFetch(last_update, ipos, 0);\n"
"  output_image.g = 1.0 / float(1u + last_frame - last.r);\n"
"  output_image.b = vertex.coordinates.x;\n"
"}\n"
;
        
        ;

      } // namespace program
    } // namespace format
  } // namespace internal
} // namespace gtulu

