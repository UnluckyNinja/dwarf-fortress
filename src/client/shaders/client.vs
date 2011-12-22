#version 330 core

uniform uint width;
uniform uint height;

in vec2 position;

out vert {
  vec2 coordinates;
} vertex;

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  
  vertex.coordinates = (position + 1.0) / 2.0;
}