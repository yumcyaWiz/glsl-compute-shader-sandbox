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
#include "bloom.h"
#include "gaussian-blur.h"
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

  Texture fragColor;
  ComputeShader initParticles;
  ComputeShader updateParticles;
  ComputeShader swapParticles;
  Shader renderParticles;

  ComputeShader extractBrightPixels;

  Texture brightColor;
  FrameBuffer frameBuffer;

  GaussianBlur gaussianBlur;

  Quad quad;
  Shader renderShader;

 public:
  Renderer()
      : resolution{512, 512},
        nParticles{30000},
        dt{0.01f},
        fragColor{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT},
        brightColor{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT},
        frameBuffer{{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}} {
    particles.setParticles(&particlesIn);

    initParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" / "n-body" /
        "init-particles.comp");
    initParticles.linkShader();

    updateParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" / "n-body" /
        "update-particles.comp");
    updateParticles.linkShader();

    swapParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" / "n-body" /
        "swap-particles.comp");
    swapParticles.linkShader();

    // NOTE: to use gl_PointSize
    glEnable(GL_PROGRAM_POINT_SIZE);
    // NOTE: blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    renderParticles.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render-particles.vert");
    renderParticles.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render-particles.frag");
    renderParticles.linkShader();

    renderShader.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderShader.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderShader.linkShader();

    // generate particles
    placeParticlesCircular();
  }

  void destroy() {
    particles.destroy();

    particlesIn.destroy();
    particlesOut.destroy();

    fragColor.destroy();
    brightColor.destroy();
    frameBuffer.destroy();

    initParticles.destroy();
    updateParticles.destroy();
    swapParticles.destroy();
    renderParticles.destroy();

    gaussianBlur.destroy();

    quad.destroy();
    renderShader.destroy();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  uint32_t getNumberOfParticles() const { return this->nParticles; }

  float getDt() const { return this->dt; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
    fragColor.setResolution(resolution);
    brightColor.setResolution(resolution);
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
          glm::vec3(-std::sin(theta), std::cos(theta), 0);

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

  void initVelocity() const {
    particlesIn.bindToShaderStorageBuffer(0);
    particlesOut.bindToShaderStorageBuffer(1);
    initParticles.setUniform("dt", dt);
    initParticles.run(std::ceil(nParticles / 128.0f), 1, 1);

    // swap in/out particles
    swapParticles.run(std::ceil(nParticles / 128.0f), 1, 1);
  }

  void move(const CameraMovement& movement_direction, float delta_time) {
    camera.move(movement_direction, delta_time);
  }

  void lookAround(float d_phi, float d_theta) {
    camera.lookAround(d_phi, d_theta);
  }

  void render() {
    // render particles
    frameBuffer.bindTexture(fragColor, 0);
    frameBuffer.activate();
    renderParticles.setUniform(
        "viewProjection",
        camera.computeViewProjectionmatrix(resolution.x, resolution.y));
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    particles.draw(renderParticles);
    frameBuffer.deactivate();

    // extract bright pixels

    // gaussian blur bright color texture
    gaussianBlur.blur(fragColor);

    // render
    fragColor.bindToTextureUnit(0);
    brightColor.bindToTextureUnit(1);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    quad.draw(renderShader);

    // update particles
    particlesIn.bindToShaderStorageBuffer(0);
    particlesOut.bindToShaderStorageBuffer(1);
    updateParticles.setUniform("dt", dt);
    updateParticles.run(std::ceil(nParticles / 128.0f), 1, 1);

    // swap in/out particles
    swapParticles.run(std::ceil(nParticles / 128.0f), 1, 1);
    // std::swap(particlesIn, particlesOut);
  }
};

#endif