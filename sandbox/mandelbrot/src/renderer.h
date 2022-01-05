#ifndef _RENDERER_H
#define _RENDERER_H
#include <algorithm>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/quad.h"
#include "gcss/texture.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;
  glm::vec2 center;
  float scale;
  uint32_t maxIterations;

  Texture texture;
  ComputeShader mandelbrotShader;

  Quad quad;
  Shader renderShader;

 public:
  Renderer()
      : resolution{512, 512},
        center{0, 0},
        scale{1.0f},
        maxIterations{100u},
        texture{glm::vec2(512, 512), GL_RGBA32F, GL_RGBA, GL_FLOAT} {
    mandelbrotShader.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "mandelbrot.comp");
    mandelbrotShader.linkShader();

    renderShader.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderShader.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderShader.linkShader();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  glm::vec2 getCenter() const { return this->center; }

  float getScale() const { return this->scale; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    // resize texture
    texture.setResolution(this->resolution);
  }

  void move(const glm::vec2& v) { center += scale * v; }

  void zoom(float delta) { scale += scale * delta; }

  void setMaxIterations(uint32_t n_iterations) {
    this->maxIterations = std::min(n_iterations, 10000u);
  }

  void render() const {
    // run compute shader
    texture.bindToImageUnit(0, GL_WRITE_ONLY);
    mandelbrotShader.setUniform("center", center);
    mandelbrotShader.setUniform("scale", scale);
    mandelbrotShader.setUniform("max_iterations", maxIterations);
    mandelbrotShader.run(std::ceil(resolution.x / 8.0f),
                         std::ceil(resolution.y / 8.0f), 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    texture.bindToTextureUnit(0);
    quad.draw(renderShader);
  }
};

#endif