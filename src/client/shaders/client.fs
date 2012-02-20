#version 330 core

in layout(origin_upper_left,pixel_center_integer) vec4 gl_FragCoord;

uniform ivec2 grid_size;
uniform ivec2 window_size;

// uniform uint last_frame;
// uniform usampler2D last_update;

uniform sampler2D characters;
uniform sampler2D texture_indexes;
uniform sampler2D texture_colors;

uniform sampler2DArray tilesets;
uniform usampler1D tilesets_info;

in vert {
  vec2 coordinates;
} vertex;

out vec4 output_image;

void main() {
  vec2 pos = vertex.coordinates;
  pos.y = 1 - pos.y;
  
  int max_grid = max(grid_size.x, grid_size.y);
  ivec3 grid_pos = ivec3(pos.x * max_grid, pos.y * max_grid, 0);
  vec3 grid_pos_off = vec3(pos.x * max_grid, pos.y * max_grid, 0.0f) - grid_pos;
  
  vec4 c = texelFetch(characters, grid_pos.yx, 0);
  vec4 tc = texelFetch(texture_colors, grid_pos.yx, 0);
  vec4 ti = texelFetch(texture_indexes, grid_pos.yx, 0);
  
  vec3 size = vec3(textureSize(tilesets, 0));
  
  uint char_id = 0u;
  uint char_index = uint(c.r * 0xFF);
  vec3 char_pos = vec3(char_index % 16u, char_index / 16u, char_id) + grid_pos_off;
  
  uvec2 char_info = texelFetch(tilesets_info, int(char_id), 0).xy;
  char_pos.x *= float(char_info.x);
  char_pos.y *= float(char_info.y);
  
  vec4 char_tile = texelFetch(tilesets, ivec3(char_pos), 0);
  
  uint tileset_id = uint(ti.r * 0xFF00) + uint(ti.g * 0xFF);
  vec3 tileset_pos = vec3(uint(ti.b * 0xFF), uint(ti.a * 0xFF), tileset_id) + grid_pos_off;
  
  uvec2 tileset_info = texelFetch(tilesets_info, int(tileset_id), 0).xy;
  tileset_pos.x *= tileset_info.x;
  tileset_pos.y *= tileset_info.y;
  
  tileset_pos.x /= size.x;
  tileset_pos.y /= size.y;
  tileset_pos.z /= size.z;
  
  vec4 tile = textureLod(tilesets, tileset_pos, 0);
  
  if(tile.r > 0.0 || tile.g > 0.0 || tile.b > 0.0 || tile.a > 0.0) {
    output_image = tile;
  } else {
    output_image = char_tile;
  }
  output_image.a = tile.r + tc.r;
}