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
  Pipeline updateCellsPipeline;

  Quad quad;
  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Pipeline renderPipeline;

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
                 GL_UNSIGNED_BYTE},
        updateCells{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                    "shaders" / "update-cells.comp"},
        vertexShader{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "shaders" / "render.vert"},
        fragmentShader{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                       "shaders" / "render.frag"} {
    updateCellsPipeline.attachComputeShader(updateCells);

    renderPipeline.attachVertexShader(vertexShader);
    renderPipeline.attachFragmentShader(fragmentShader);

    randomizeCells();
  }

  // randomize input cell
  void randomizeCells() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(0, 1);
    std::vector<uint8_t> input_cell_image(resolution.x * resolution.y);
    for (std::size_t i = 0; i < input_cell_image.size(); ++i) {
      input_cell_image[i] = dist(mt) > 0.5 ? 1 : 0;
    }
    cellsIn.setImage(input_cell_image, resolution, GL_R8UI, GL_RED_INTEGER,
                     GL_UNSIGNED_BYTE);
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  glm::vec2 getOffset() const { return this->offset; }

  float getScale() const { return this->scale; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
    this->offset = 0.5f * glm::vec2(resolution);

    cellsIn.resize(resolution);
    cellsOut.resize(resolution);

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
    fragmentShader.setUniform("offset", offset);
    fragmentShader.setUniform("scale", scale);
    quad.draw(renderPipeline);

    // limit framerate
    elapsed_time += delta_time;
    if (elapsed_time > 1.0f / fps) {
      elapsed_time = 0;

      // update input cells
      cellsIn.bindToImageUnit(0, GL_READ_ONLY);
      cellsOut.bindToImageUnit(1, GL_WRITE_ONLY);
      updateCellsPipeline.activate();
      glDispatchCompute(std::ceil(resolution.x / 8.0f),
                        std::ceil(resolution.y / 8.0f), 1);
      updateCellsPipeline.deactivate();

      // swap input/output texture
      std::swap(cellsIn, cellsOut);
    }
  }
};

#endif