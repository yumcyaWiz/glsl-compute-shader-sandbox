#ifndef _BLOOM_H
#define _BLOOM_H
#include "gaussian-blur.h"
#include "gcss/shader.h"
#include "gcss/texture.h"

class Bloom {
 private:
  GaussianBlur blur;

  Texture fragColor;
  Texture brightColor;
  ComputeShader extractBrightColor;
  ComputeShader additiveBlend;

 public:
  Bloom()
      : fragColor{glm::uvec2(512, 512), GL_RGBA32F, GL_RGBA, GL_FLOAT},
        brightColor{glm::uvec2(512, 512), GL_RGBA32F, GL_RGBA, GL_FLOAT} {
    extractBrightColor.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "post-process" / "extract-bright-color.comp");
    extractBrightColor.linkShader();

    additiveBlend.setComputeShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "shaders" /
        "post-process" / "additive-blend.comp");
    additiveBlend.linkShader();
  }

  void destroy() {
    blur.destroy();
    fragColor.destroy();
    brightColor.destroy();
    extractBrightColor.destroy();
    additiveBlend.destroy();
  }

  void bloom(const Texture& tex_in) {
    // detect resolution change
    const glm::uvec2 resolution = tex_in.getResolution();
    if (fragColor.getResolution() != resolution) {
      fragColor.setResolution(resolution);
    }
    if (brightColor.getResolution() != resolution) {
      brightColor.setResolution(resolution);
    }

    // extract bright color
    tex_in.bindToImageUnit(0, GL_READ_ONLY);
    fragColor.bindToImageUnit(1, GL_WRITE_ONLY);
    brightColor.bindToImageUnit(2, GL_WRITE_ONLY);
    extractBrightColor.setUniform("threshold", 0.1f);
    extractBrightColor.run(std::ceil(resolution.x / 8.0f),
                           std::ceil(resolution.y / 8.0f), 1);

    // gaussian blur bright color texture
    blur.blur(brightColor);

    // additive blending
    fragColor.bindToImageUnit(0, GL_READ_ONLY);
    brightColor.bindToImageUnit(1, GL_READ_ONLY);
    tex_in.bindToImageUnit(2, GL_WRITE_ONLY);
    additiveBlend.run(std::ceil(resolution.x / 8.0f),
                      std::ceil(resolution.y / 8.0f), 1);
  }
};

#endif