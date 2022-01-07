#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in float mass;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

uniform mat4 viewProjection;

void main() {
  gl_Position = viewProjection * vec4(position.xyz, 1.0);
}