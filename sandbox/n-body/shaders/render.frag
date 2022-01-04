#version 460 core

layout(binding = 0) uniform sampler2D fragColor;
layout(binding = 1) uniform sampler2D brightColor;

in vec2 texCoords;

out vec4 fs_out;

void main() {
  fs_out = texture(fragColor, texCoords);
}