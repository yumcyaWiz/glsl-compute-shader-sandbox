#ifndef _RENDERER_H
#define _RENDERER_H

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "css/quad.h"

using namespace css;

class Renderer {
 private:
  glm::vec2 resolution;

  Quad quad;
  Shader mandelbrotShader;
  Shader renderShader;

 public:
  Renderer() : resolution{512, 512} {
    renderShader.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderShader.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderShader.linkShader();
  }

  void setResolution(const glm::vec2& resolution) {
    this->resolution = resolution;
  }

  void render() const {
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    quad.draw(renderShader);
  }
};

#endif