#version 460 core

layout(binding = 0) uniform usampler2D cell;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  uint status = texture(cell, texCoords).x;
  fragColor = vec4(status * vec3(0, 1, 0), 1);
}