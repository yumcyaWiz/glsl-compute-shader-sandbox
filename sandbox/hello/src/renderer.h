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
  ComputeShader helloShader;
  Shader renderShader;

 public:
  Renderer()
      : resolution{512, 512},
        texture{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT} {
    helloShader.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "hello.comp");
    helloShader.linkShader();

    renderShader.setVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.vert");
    renderShader.setFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "render.frag");
    renderShader.linkShader();
  }

  void destroy() {
    texture.destroy();
    quad.destroy();
    helloShader.destroy();
    renderShader.destroy();
  }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    // resize texture
    texture.setResolution(this->resolution);
  }

  void render() const {
    // run compute shader
    texture.bindToImageUnit(0, GL_WRITE_ONLY);
    helloShader.run(resolution.x, resolution.y, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    texture.bindToTextureUnit(0);
    quad.draw(renderShader);
  }
};

#endif