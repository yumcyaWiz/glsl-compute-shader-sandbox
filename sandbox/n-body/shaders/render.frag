#version 460 core

out vec4 fragColor;

void main() {
  float dist_from_center = length(gl_PointCoord - vec2(0.5));
  vec4 color = 0.01 * 1.0 / (dist_from_center * dist_from_center) * vec4(0.4, 0.8, 1.0, 1.0);
  fragColor = color;
}