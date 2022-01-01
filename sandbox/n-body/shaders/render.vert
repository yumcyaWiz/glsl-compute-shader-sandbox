#version 460 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 velocity;

out vec2 texCoords;

uniform mat4 viewProjection;

void main() {
  // gl_Position = viewProjection * vec4(position.xyz, 1.0);
  gl_Position = vec4(position.xyz, 1.0);
}