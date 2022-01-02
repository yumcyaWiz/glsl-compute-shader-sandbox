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
//
#include "particles.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;
  uint32_t nParticles;

  Camera camera;

  Buffer particlesIn;
  Buffer particlesOut;

  Particles particles;

  ComputeShader updateParticles;
  ComputeShader swapParticles;
  Shader renderParticles;

 public:
  Renderer()
      : resolution{512, 512}, nParticles{30000}, particles{&particlesIn} {
    updateParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "update-particles.comp");
    updateParticles.linkShader();

    swapParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "swap-particles.comp");
    swapParticles.linkShader();

    renderParticles.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderParticles.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderParticles.linkShader();

    // populate particles buffer
    // randomizeParticles();
    placeParticlesCircle();
  }

  void destroy() {
    particles.destroy();
    particlesIn.destroy();
    particlesOut.destroy();
    updateParticles.destroy();
    swapParticles.destroy();
    renderParticles.destroy();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  void placeParticlesCircle() {
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

      const float r = u + 0.001f;
      const float theta = 2.0f * 3.14f * v;

      const glm::vec3 position =
          r * glm::vec3(std::cos(theta), std::sin(theta), 0) +
          0.01f * glm::vec3(dist(mt), dist(mt), dist(mt));
      const glm::vec3 velocity =
          glm::vec3(-std::sin(theta), std::cos(theta), 0);

      data[idx].position = glm::vec4(position, 0);
      data[idx].velocity = glm::vec4(velocity, 0);
      data[idx].mass = 100.0f;
    }
    particlesIn.setData(data, GL_DYNAMIC_DRAW);
    particlesOut.setData(data, GL_DYNAMIC_DRAW);
  }

  void randomizeParticles() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(-1, 1);

    const int grid_size = std::sqrt(nParticles);
    std::vector<Particle> data(nParticles);
    for (std::size_t idx = 0; idx < data.size(); ++idx) {
      const int i = idx % grid_size;
      const int j = (idx / grid_size) % grid_size;
      const float u = (2.0f * i) / grid_size - 1.0f;
      const float v = (2.0f * j) / grid_size - 1.0f;

      const glm::vec3 position =
          glm::vec3(u, v, 0) + 0.01f * glm::vec3(dist(mt), dist(mt), dist(mt));
      const glm::vec3 velocity =
          0.2f * (glm::cross(glm::normalize(-position), glm::vec3(0, 0, 1)) +
                  (-position));

      data[idx].position = glm::vec4(position, 0);
      data[idx].velocity = glm::vec4(velocity, 0);
      data[idx].mass = 10000.0f * 0.5f * (dist(mt) + 1.0f);
      // data[idx].mass = 10000.0f;
    }

    particlesIn.setData(data, GL_DYNAMIC_DRAW);
    particlesOut.setData(data, GL_DYNAMIC_DRAW);
  }

  void initVelocity() const {}

  void move(const CameraMovement& movement_direction, float delta_time) {
    camera.move(movement_direction, delta_time);
  }

  void lookAround(float d_phi, float d_theta) {
    camera.lookAround(d_phi, d_theta);
  }

  void render() {
    // render particles
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    renderParticles.setUniform(
        "viewProjection",
        camera.computeViewProjectionmatrix(resolution.x, resolution.y));
    particles.draw(renderParticles);

    // update particles
    particlesIn.bindToShaderStorageBuffer(0);
    particlesOut.bindToShaderStorageBuffer(1);
    updateParticles.setUniform("dt", 0.01f);
    updateParticles.run(std::ceil(nParticles / 128.0f), 1, 1);

    // swap in/out particles
    swapParticles.run(std::ceil(nParticles / 128.0f), 1, 1);
    // std::swap(particlesIn, particlesOut);
  }
};

#endif