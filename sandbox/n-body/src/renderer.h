#ifndef _RENDERER_H
#define _RENDERER_H
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/buffer.h"
#include "gcss/camera.h"
#include "gcss/quad.h"
#include "gcss/shader.h"
#include "gcss/vertex-array-object.h"
//
#include "particles.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;
  uint32_t nParticles;
  float dt;

  Camera camera;

  Particles particles;
  Buffer particlesIn;
  Buffer particlesOut;

  ComputeShader initParticles;
  Program initParticlesProgram;
  ComputeShader updateParticles;
  Program updateParticlesProgram;

  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Program renderProgram;

 public:
  Renderer() : resolution{512, 512}, nParticles{30000}, dt{0.01f} {
    particles.setParticles(&particlesIn);

    initParticles.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                          "shaders" / "n-body" / "init-particles.comp");
    initParticlesProgram.attachShader(initParticles);
    initParticlesProgram.linkProgram();

    updateParticles.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                            "shaders" / "n-body" / "update-particles.comp");
    updateParticlesProgram.attachShader(updateParticles);
    updateParticlesProgram.linkProgram();

    // NOTE: to use gl_PointSize
    glEnable(GL_PROGRAM_POINT_SIZE);
    // NOTE: blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    vertexShader.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                         "shaders" / "render-particles.vert");
    fragmentShader.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                           "shaders" / "render-particles.frag");
    renderProgram.attachShader(vertexShader);
    renderProgram.attachShader(fragmentShader);
    renderProgram.linkProgram();

    // generate particles
    placeParticlesCircular();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  uint32_t getNumberOfParticles() const { return this->nParticles; }

  float getDt() const { return this->dt; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  void setNumberOfParticles(uint32_t nParticles) {
    this->nParticles = nParticles;

    // regenerate particles
    placeParticlesCircular();
  }

  void setDt(float dt) { this->dt = dt; }

  void resetParticles() { placeParticlesCircular(); }

  void placeParticlesCircular() {
    const float black_hole_mass = 100000;

    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(-1, 1);

    std::vector<Particle> data(nParticles);
    const int grid_size = std::sqrt(nParticles);
    for (std::size_t idx = 0; idx < data.size(); ++idx) {
      const int i = idx % grid_size;
      const int j = (idx / grid_size) % grid_size;
      const float u = static_cast<float>(i) / grid_size;
      const float v = static_cast<float>(j) / grid_size;

      const float r = 0.5f * (u + 0.1f * (dist(mt) + 1.0f));
      const float theta = 2.0f * 3.14f * v + 0.1f * dist(mt);

      const float mass = 1.0f * 0.5f * (dist(mt) + 1.0f);
      const glm::vec3 position =
          r * glm::vec3(std::cos(theta), std::sin(theta), 0.1f * dist(mt));
      const float G = 6.67430e-11;
      const glm::vec3 velocity =
          std::sqrt((G * black_hole_mass) / r) *
          (glm::vec3(-std::sin(theta), std::cos(theta), 0));

      data[idx].position = glm::vec4(position, 0);
      data[idx].velocity = glm::vec4(velocity, 0);
      data[idx].mass = mass;
    }
    data[0].position = glm::vec4(0);
    data[0].velocity = glm::vec4(0);
    data[0].mass = black_hole_mass;

    particlesIn.setData(data, GL_DYNAMIC_DRAW);
    particlesOut.setData(data, GL_DYNAMIC_DRAW);

    initVelocity();
  }

  void initVelocity() {
    particlesIn.bindToShaderStorageBuffer(0);
    particlesOut.bindToShaderStorageBuffer(1);
    initParticlesProgram.setUniform("dt", dt);

    initParticlesProgram.activate();
    glDispatchCompute(std::ceil(nParticles / 128.0f), 1, 1);
    initParticlesProgram.deactivate();

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // swap in/out particles
    std::swap(particlesIn, particlesOut);
  }

  void move(const CameraMovement& movement_direction, float delta_time) {
    camera.move(movement_direction, delta_time);
  }

  void lookAround(float d_phi, float d_theta) {
    camera.lookAround(d_phi, d_theta);
  }

  void render() {
    // render particles
    renderProgram.setUniform(
        "viewProjection",
        camera.computeViewProjectionmatrix(resolution.x, resolution.y));
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    particles.draw(renderProgram);

    // update particles
    particlesIn.bindToShaderStorageBuffer(0);
    particlesOut.bindToShaderStorageBuffer(1);
    updateParticlesProgram.setUniform("dt", dt);

    updateParticlesProgram.activate();
    glDispatchCompute(std::ceil(nParticles / 128.0f), 1, 1);
    updateParticlesProgram.deactivate();

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // swap in/out particles
    std::swap(particlesIn, particlesOut);
  }
};

#endif