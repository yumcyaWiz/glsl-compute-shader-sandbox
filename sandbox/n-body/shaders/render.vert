#version 460 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 velocity;

uniform mat4 viewProjection;

void main() {
  gl_Position = viewProjection * vec4(position.xyz, 1.0);
}