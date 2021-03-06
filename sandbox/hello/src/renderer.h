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
  ComputeShader paintTexture;
  Pipeline paintTexturePipeline;

  Quad quad;
  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Pipeline renderPipeline;

 public:
  Renderer()
      : resolution{512, 512},
        texture{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT},
        paintTexture(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "shaders" / "hello.comp"),
        vertexShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "shaders" / "render.vert"),
        fragmentShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                       "shaders" / "render.frag") {
    paintTexturePipeline.attachComputeShader(paintTexture);

    renderPipeline.attachVertexShader(vertexShader);
    renderPipeline.attachFragmentShader(fragmentShader);
  }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
    texture.resize(resolution);
  }

  void render() const {
    // run compute shader
    texture.bindToImageUnit(0, GL_WRITE_ONLY);
    paintTexturePipeline.activate();
    glDispatchCompute(resolution.x, resolution.y, 1);
    paintTexturePipeline.deactivate();

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    texture.bindToTextureUnit(0);
    quad.draw(renderPipeline);
  }
};

#endif