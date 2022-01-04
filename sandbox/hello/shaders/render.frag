#version 460 core

layout(binding = 0) uniform sampler2D tex;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  fragColor = texture(tex, texCoords);
}