#version 460 core

layout(r8ui, binding = 0) uniform uimage2D cells;
uniform vec2 offset;
uniform float scale;

in vec2 texCoords;

out vec4 fragColor;

void main() {
  ivec2 size = imageSize(cells);
  vec2 uv = (2.0 * gl_FragCoord.xy - size) / size;

  ivec2 idx = ivec2(offset + uv * 0.5 * size / scale);
  uint status = imageLoad(cells, idx).x;

  fragColor = vec4(status * vec3(0, 1, 0), 1);
}