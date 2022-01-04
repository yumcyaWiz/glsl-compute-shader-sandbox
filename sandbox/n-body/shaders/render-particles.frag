#version 460 core

in vec3 base_color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main() {
  vec2 uv = 2.0 * gl_PointCoord - vec2(1);
  float r = length(uv);
  vec3 color = r < 1.0 ? 0.001 / (r * r) * base_color : vec3(0);
  fragColor = vec4(color, 1.0);

  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
  brightColor = brightness > 1.0 ? vec4(color, 1.0) : vec4(vec3(0), 1.0);
}