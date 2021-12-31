#ifndef _RENDERER_H
#define _RENDERER_H

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/quad.h"
#include "gcss/shader.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;

  Quad quad;
  ComputeShader updateParticles;
  ComputeShader swapParticles;
  Shader renderShader;

 public:
  Renderer() : resolution{512, 512} {
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
  }

  void destroy() {
    quad.destroy();
    updateParticles.destroy();
    swapParticles.destroy();
    renderShader.destroy();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  void render() const {
    updateParticles.run(resolution.x / 8, resolution.y / 8, 1);

    swapParticles.run(resolution.x / 8, resolution.y / 8, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    quad.draw(renderShader);
  }
};

#endif