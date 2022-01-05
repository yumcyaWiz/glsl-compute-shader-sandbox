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
  double elapsed_time;

  Texture cellsIn;
  Texture cellsOut;
  ComputeShader updateCells;
  Program updateCellsProgram;

  Quad quad;
  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Program renderProgram;

 public:
  Renderer()
      : resolution{512, 512},
        offset{256, 256},
        scale{1},
        fps{24},
        elapsed_time{0},
        cellsIn{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                GL_UNSIGNED_BYTE},
        cellsOut{glm::uvec2(512, 512), GL_R8UI, GL_RED_INTEGER,
                 GL_UNSIGNED_BYTE} {
    updateCells.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                        "shaders" / "update-cells.comp");
    updateCellsProgram.attachShader(updateCells);
    updateCellsProgram.linkProgram();

    vertexShader.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                         "shaders" / "render.vert");
    fragmentShader.compile(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                           "shaders" / "render.frag");
    renderProgram.attachShader(vertexShader);
    renderProgram.attachShader(fragmentShader);
    renderProgram.linkProgram();

    randomizeCells();
  }

  // randomize input cell
  void randomizeCells() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(0, 1);
    std::vector<uint8_t> input_cell_image(this->resolution.x *
                                          this->resolution.y);
    for (std::size_t i = 0; i < input_cell_image.size(); ++i) {
      input_cell_image[i] = dist(mt) > 0.5 ? 1 : 0;
    }
    cellsIn.setImage(input_cell_image);
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  glm::vec2 getOffset() const { return this->offset; }

  float getScale() const { return this->scale; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
    this->offset = 0.5f * glm::vec2(resolution);

    cellsIn.setResolution(this->resolution);
    cellsOut.setResolution(this->resolution);

    randomizeCells();
  }

  void move(const glm::vec2& delta) { this->offset += delta / this->scale; }

  void zoom(const float delta) { this->scale += this->scale * delta; }

  void setFPS(uint32_t fps) { this->fps = fps; }

  void render(float delta_time) {
    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    cellsIn.bindToImageUnit(0, GL_READ_ONLY);
    renderProgram.setUniform("offset", offset);
    renderProgram.setUniform("scale", scale);
    quad.draw(renderProgram);

    // limit framerate
    elapsed_time += delta_time;
    if (elapsed_time > 1.0f / fps) {
      elapsed_time = 0;

      // update input cells
      cellsIn.bindToImageUnit(0, GL_READ_ONLY);
      cellsOut.bindToImageUnit(1, GL_WRITE_ONLY);
      updateCellsProgram.activate();
      glDispatchCompute(std::ceil(resolution.x / 8.0f),
                        std::ceil(resolution.y / 8.0f), 1);
      updateCellsProgram.deactivate();

      // swap input/output texture
      std::swap(cellsIn, cellsOut);
    }
  }
};

#endif