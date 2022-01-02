#version 460 core

out vec4 fragColor;

void main() {
  vec2 uv = 2.0 * gl_PointCoord - vec2(1);
  float r = length(uv);
  vec3 color = r < 1.0 ? 0.01 / r * vec3(1, 235.0 / 255, 209.0 / 255) : vec3(0);
  fragColor = vec4(color, 1.0);
}