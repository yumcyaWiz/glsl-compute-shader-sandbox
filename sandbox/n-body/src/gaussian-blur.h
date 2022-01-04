#ifndef _GAUSSIAN_BLUR_H
#define _GAUSSIAN_BLUR_H
#include "gcss/shader.h"
#include "gcss/texture.h"

using namespace gcss;

class GaussianBlur {
 private:
  ComputeShader shader;

 public:
  GaussianBlur() {
    shader.setComputeShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                            "shaders" / "gaussian-blur.comp");
    shader.linkShader();
  }
  ~GaussianBlur() { shader.destroy(); }

  void blur(const Texture& tex_in) const {
    // prepare output texture
    const glm::uvec2 resolution = tex_in.getResolution();
    Texture tex_out{resolution, tex_in.getInternalFormat(), tex_in.getFormat(),
                    tex_in.getType()};

    // run two pass gaussian blur
    // blur horizontally
    tex_in.bindToImageUnit(0, GL_READ_ONLY);
    tex_out.bindToImageUnit(1, GL_WRITE_ONLY);
    shader.setUniform("horizontal", true);
    shader.run(std::ceil(resolution.x / 8.0f), std::ceil(resolution.y / 8.0f),
               1);
    // blur vertically
    tex_out.bindToImageUnit(0, GL_READ_ONLY);
    tex_in.bindToImageUnit(1, GL_WRITE_ONLY);
    shader.setUniform("horizontal", false);
    shader.run(std::ceil(resolution.x / 8.0f), std::ceil(resolution.y / 8.0f),
               1);
  };
};

#endif