#version 460 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoords;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out vec2 texCoords;

void main() {
  texCoords = vTexCoords;
  gl_Position = vec4(vPosition, 1.0);
}