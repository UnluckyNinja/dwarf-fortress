#version 330 core

in layout(origin_upper_left,pixel_center_integer) vec4 gl_FragCoord;

uniform uint width;
uniform uint height;

uniform uint last_frame;
uniform usampler2D last_update;

uniform sampler2D characters;
uniform sampler2D texture_colors;
uniform sampler2D texture_indexes;

in vert {
  vec2 coordinates;
} vertex;

out vec4 output_image;

void main() {
  vec2 size = textureSize(characters, 0);
  vec2 pos = vertex.coordinates;
  uint maxsize = max(width, height);
  pos.x = pos.x * maxsize;
  pos.y = (1 - pos.y) * maxsize;
  ivec2 ipos = ivec2(pos.y, pos.x);
  
  vec4 c = texelFetch(characters, ipos, 0);
  vec4 tc = texelFetch(texture_colors, ipos, 0);
  vec4 ti = texelFetch(texture_indexes, ipos, 0);
  output_image = c + tc + ti;

  uvec4 last = texelFetch(last_update, ipos, 0);
  output_image.g = 1.0 / float(1u + last_frame - last.r);
  output_image.b = vertex.coordinates.x;
}