#ifndef _RENDERER_H
#define _RENDERER_H
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/buffer.h"
#include "gcss/quad.h"
#include "gcss/shader.h"

using namespace gcss;

struct Particle {
  glm::vec4 position;
  glm::vec4 velocity;
};

class Renderer {
 private:
  glm::uvec2 resolution;
  uint32_t nParticles;

  ShaderStorageBuffer inputParticles;
  ShaderStorageBuffer outputParticles;

  Quad quad;
  ComputeShader updateParticles;
  ComputeShader swapParticles;
  Shader renderShader;

 public:
  Renderer() : resolution{512, 512}, nParticles{10000} {
    updateParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "update-particles.comp");
    updateParticles.linkShader();

    swapParticles.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "swap-particles.comp");
    swapParticles.linkShader();

    renderShader.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderShader.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderShader.linkShader();

    randomizeParticles();
  }

  void destroy() {
    quad.destroy();
    inputParticles.destroy();
    outputParticles.destroy();
    updateParticles.destroy();
    swapParticles.destroy();
    renderShader.destroy();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  void randomizeParticles() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(-1, 1);

    std::vector<Particle> data(nParticles);
    for (int i = 0; i < data.size(); ++i) {
      data[i].position = glm::vec4(dist(mt), dist(mt), dist(mt), 0);
      data[i].velocity = glm::vec4(dist(mt), dist(mt), dist(mt), 0);
    }

    inputParticles.setData(data);
    outputParticles.setData(data);
  }

  void render() const {
    inputParticles.bind(0);
    outputParticles.bind(1);
    updateParticles.run(nParticles / 128, 1, 1);

    swapParticles.run(nParticles / 128, 1, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    quad.draw(renderShader);
  }
};

#endif