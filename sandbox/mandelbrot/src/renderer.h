#ifndef _RENDERER_H
#define _RENDERER_H

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/quad.h"
#include "gcss/texture.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;

  Texture texture;

  Quad quad;
  ComputeShader mandelbrotShader;
  Shader renderShader;

 public:
  Renderer() : resolution{512, 512} {
    texture = Texture(resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT);

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

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    // resize texture
    texture.setResolution(this->resolution);
  }

  void render() const {
    // run compute shader
    mandelbrotShader.setImageTexture(texture, 0, GL_WRITE_ONLY);
    mandelbrotShader.setUniform("center", glm::vec2(0));
    mandelbrotShader.setUniform("scale", 1.0f);
    mandelbrotShader.setUniform("max_iterations", 100u);
    mandelbrotShader.run(resolution.x / 8, resolution.y / 8, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    renderShader.setTexture("tex", texture, 0);
    quad.draw(renderShader);
  }
};

#endif