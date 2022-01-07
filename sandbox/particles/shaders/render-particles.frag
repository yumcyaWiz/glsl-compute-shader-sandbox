#version 460 core

out vec4 fragColor;

uniform vec3 baseColor;

void main() {
  vec2 uv = 2.0 * gl_PointCoord - vec2(1);
  float r = length(uv);
  vec3 color = baseColor * exp(-10.0 * r);
  fragColor = vec4(color, 1.0);
}