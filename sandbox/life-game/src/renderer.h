#ifndef _RENDERER_H
#define _RENDERER_H
#include <algorithm>
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/quad.h"
#include "gcss/texture.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;

  Texture inputCell;
  Texture outputCell;

  Quad quad;
  ComputeShader lifeGameShader;
  Shader renderShader;

 public:
  Renderer()
      : resolution{512, 512},
        inputCell{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                  GL_UNSIGNED_BYTE},
        outputCell{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                   GL_UNSIGNED_BYTE} {
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

    randomizeCells();
  }

  // randomize input cell
  void randomizeCells() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(0, 1);
    std::vector<uint8_t> input_cell_image(this->resolution.x *
                                          this->resolution.y);
    for (int i = 0; i < input_cell_image.size(); ++i) {
      input_cell_image[i] = dist(mt) > 0.5 ? 1 : 0;
    }
    inputCell.setImage(input_cell_image);
  }

  void destroy() {
    inputCell.destroy();
    outputCell.destroy();
    quad.destroy();
    lifeGameShader.destroy();
    renderShader.destroy();
  }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    inputCell.setResolution(this->resolution);
    outputCell.setResolution(this->resolution);

    randomizeCells();
  }

  void render() const {
    // run compute shader
    lifeGameShader.setImageTexture(inputCell, 0, GL_READ_ONLY);
    lifeGameShader.setImageTexture(outputCell, 1, GL_WRITE_ONLY);
    lifeGameShader.run(resolution.x / 8, resolution.y / 8, 1);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    renderShader.setTexture("cell", outputCell, 0);
    quad.draw(renderShader);
  }
};

#endif