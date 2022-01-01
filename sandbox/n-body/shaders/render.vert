#version 460 core
layout (location = 0) in vec3 position;

out vec2 texCoords;

uniform mat4 viewProjection;

void main() {
  gl_Position = viewProjection * vec4(position, 1.0);
}