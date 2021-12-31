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
  glm::vec2 offset;
  float scale;
  uint32_t fps;

  Texture inputCells;
  Texture outputCells;

  Quad quad;
  ComputeShader updateCells;
  ComputeShader swapCells;
  Shader renderShader;

  double elapsed_time;

 public:
  Renderer()
      : resolution{512, 512},
        offset{256, 256},
        scale{1},
        elapsed_time{0},
        inputCells{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                   GL_UNSIGNED_BYTE},
        outputCells{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                    GL_UNSIGNED_BYTE} {
    updateCells.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "update-cells.comp");
    updateCells.linkShader();

    swapCells.setComputeShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                               "shaders" / "swap-cells.comp");
    swapCells.linkShader();

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
    inputCells.setImage(input_cell_image);
  }

  void destroy() {
    inputCells.destroy();
    outputCells.destroy();
    quad.destroy();
    updateCells.destroy();
    swapCells.destroy();
    renderShader.destroy();
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  glm::vec2 getOffset() const { return this->offset; }

  float getScale() const { return this->scale; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
    this->offset = 0.5f * glm::vec2(resolution);

    inputCells.setResolution(this->resolution);
    outputCells.setResolution(this->resolution);

    randomizeCells();
  }

  void move(const glm::vec2& delta) { this->offset += delta / this->scale; }

  void zoom(const float delta) { this->scale += this->scale * delta; }

  void setFPS(uint32_t fps) { this->fps = fps; }

  void render(float delta_time) {
    // limit framerate
    elapsed_time += delta_time;
    if (elapsed_time > 1.0f / fps) {
      elapsed_time = 0;

      // update input cells
      inputCells.bindToImageUnit(0, GL_READ_ONLY);
      outputCells.bindToImageUnit(1, GL_WRITE_ONLY);
      updateCells.run(resolution.x / 8, resolution.y / 8, 1);

      // swap input cells and output cells
      inputCells.bindToImageUnit(0, GL_READ_ONLY);
      outputCells.bindToImageUnit(1, GL_WRITE_ONLY);
      swapCells.run(resolution.x / 8, resolution.y / 8, 1);
    }

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    outputCells.bindToImageUnit(0, GL_READ_ONLY);
    renderShader.setUniform("offset", offset);
    renderShader.setUniform("scale", scale);
    quad.draw(renderShader);
  }
};

#endif