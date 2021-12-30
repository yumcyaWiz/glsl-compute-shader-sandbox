#version 460 core

uniform vec2 resolution;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  fragColor = vec4(texCoords, 0.0, 1.0);
}