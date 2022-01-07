#version 460 core

out vec4 fragColor;

uniform vec3 baseColor;

void main() {
  fragColor = vec4(baseColor, 1.0);
}