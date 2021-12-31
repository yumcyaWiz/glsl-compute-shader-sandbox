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
  glm::vec2 resolution;

  Texture texture;

  Quad quad;
  Shader helloShader;
  Shader renderShader;

 public:
  Renderer() : resolution{512, 512} {
    texture = Texture(resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    helloShader.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "hello.comp");

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

    helloShader.setImageTexture(texture, 0, GL_WRITE_ONLY);

    quad.draw(renderShader);
  }
};

#endif