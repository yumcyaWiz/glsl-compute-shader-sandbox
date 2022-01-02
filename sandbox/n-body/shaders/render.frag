#version 460 core

out vec4 fragColor;

void main() {
  vec2 uv = 2.0 * gl_PointCoord - vec2(1);
  float r = length(uv);
  vec3 color = r < 1.0 ? 0.01 / r * vec3(0.6, 0.8, 1.0) : vec3(0);

  // reinhard
  color = color / (vec3(1) + color);

  fragColor = vec4(color, 1.0);
}