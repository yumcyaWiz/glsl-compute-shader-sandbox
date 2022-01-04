#ifndef _GAUSSIAN_BLUR_H
#define _GAUSSIAN_BLUR_H
#include "gcss/shader.h"
#include "gcss/texture.h"

using namespace gcss;

class GaussianBlur {
 private:
  ComputeShader shader;
  Texture texOut;

 public:
  GaussianBlur() : texOut{glm::uvec2(512, 512), GL_RGBA32F, GL_RGBA, GL_FLOAT} {
    shader.setComputeShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                            "shaders" / "post-process" / "gaussian-blur.comp");
    shader.linkShader();
  }

  void destroy() {
    shader.destroy();
    texOut.destroy();
  }

  void blur(const Texture& tex_in) {
    // detect resolution change
    const glm::uvec2 resolution = tex_in.getResolution();
    if (texOut.getResolution() != resolution) {
      texOut.setResolution(resolution);
    }

    // run two pass gaussian blur
    // blur horizontally
    tex_in.bindToImageUnit(0, GL_READ_ONLY);
    texOut.bindToImageUnit(1, GL_WRITE_ONLY);
    shader.setUniform("horizontal", true);
    shader.run(std::ceil(resolution.x / 8.0f), std::ceil(resolution.y / 8.0f),
               1);
    // blur vertically
    texOut.bindToImageUnit(0, GL_READ_ONLY);
    tex_in.bindToImageUnit(1, GL_WRITE_ONLY);
    shader.setUniform("horizontal", false);
    shader.run(std::ceil(resolution.x / 8.0f), std::ceil(resolution.y / 8.0f),
               1);
  };
};

#endif