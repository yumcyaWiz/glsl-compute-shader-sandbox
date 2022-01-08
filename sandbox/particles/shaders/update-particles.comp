#version 460 core
layout(local_size_x = 128) in;

struct Particle {
  vec4 position;
  vec4 velocity;
  float mass;
};

uniform vec3 gravityCenter;
uniform float gravityIntensity;
uniform float k;
uniform bool increaseK;
uniform float dt;

layout(std430, binding = 0) buffer layout_particles {
  Particle particles[];
};

const float EPS = 1e-3;

void main() {
  uint gidx = gl_GlobalInvocationID.x;

  vec3 position = particles[gidx].position.xyz;
  vec3 velocity = particles[gidx].velocity.xyz;
  float mass = particles[gidx].mass;

  // compute gravitational force
  vec3 v = gravityCenter - position;
  float l = length(v);
  float k = increaseK ? 10.0 * k : k;
  vec3 F = mass * gravityIntensity * v / (l * l + EPS) - k * velocity;

  // leap-frog scehem
  vec3 a = F / mass;
  vec3 velocity_next = velocity + a * dt;
  vec3 position_next = position + velocity_next * dt;

  // update position, velocity
  particles[gidx].position.xyz = position_next;
  particles[gidx].velocity.xyz = velocity_next;
}