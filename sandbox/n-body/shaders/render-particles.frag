#version 460 core

in vec3 base_color;

layout(location = 0) out vec4 fragColor;

void main() {
  vec2 uv = 2.0 * gl_PointCoord - vec2(1);
  float r = length(uv);
  vec3 color = r < 1.0 ? 0.005 / r * base_color : vec3(0);
  fragColor = vec4(color, 1.0);
}