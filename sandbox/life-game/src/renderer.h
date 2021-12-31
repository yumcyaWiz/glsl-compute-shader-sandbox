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

  Texture texture;

  Quad quad;
  ComputeShader lifeGameShader;
  Shader renderShader;

 public:
  Renderer() : texture{glm::vec2(512, 512), GL_RGBA32F, GL_RGBA, GL_FLOAT} {
    lifeGameShader.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "life-game.comp");
    lifeGameShader.linkShader();

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
    lifeGameShader.destroy();
    renderShader.destroy();
  }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    // resize texture
    texture.setResolution(this->resolution);
  }

  void render() const {
    // run compute shader
    lifeGameShader.setImageTexture(texture, 0, GL_WRITE_ONLY);
    lifeGameShader.run(resolution.x / 8, resolution.y / 8, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    renderShader.setTexture("tex", texture, 0);
    quad.draw(renderShader);
  }
};

#endif