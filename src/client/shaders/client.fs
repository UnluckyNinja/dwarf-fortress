#version 330 core

uniform uint width;
uniform uint height;

uniform float last_frame;
uniform sampler2D last_update;

uniform sampler2D characters;
uniform sampler2D tiles;
uniform sampler2D tiles_color;

uniform bool use_tile;

uniform uvec2 tile_size;
uniform sampler3D tilesets;

in vert {
  vec2 coordinates;
} vertex;

out vec4 output_image;

void main() {
  vec2 size = textureSize(characters, 0);
  vec2 pos = vertex.coordinates;
  pos.x = pos.x * width;
  pos.y = (1 - pos.y) * height;
  ivec2 ipos = ivec2(pos.y, pos.x);
  
  vec4 c = texelFetch(characters, ipos, 0);
  if (use_tile) {
    vec4 t = texture(tiles, pos);
    vec4 t_color = texture(tiles_color, pos);
    vec4 o = texture(tilesets, vec3(pos, t.r));
    o.r += t_color.r;
    o.g += t_color.g;
    o.b += t_color.b;
    output_image = o;
  } else {
    output_image = c;
  }

  vec4 last = texelFetch(last_update, ipos, 0);
  output_image.a = (last.r / last_frame) * (last.r / last_frame);
}