#ifndef _RENDERER_H
#define _RENDERER_H

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/quad.h"
#include "gcss/texture.h"

using namespace gcss;

enum class ToneMappingType : int {
  LINEAR = 0,
  REINHARD = 1,
  ACES = 2,
  UCHIMURA = 3
};

class Renderer {
 private:
  glm::uvec2 resolution;
  float exposure;
  bool toneMappingOnRGB;
  ToneMappingType toneMappingType;
  float gamma;

  Texture textureIn;
  Texture textureOut;
  ComputeShader toneMapping;
  Pipeline toneMappingPipeline;

  Quad quad;
  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Pipeline renderPipeline;

 public:
  Renderer()
      : resolution{512, 512},
        exposure{1.0f},
        toneMappingOnRGB{false},
        toneMappingType{ToneMappingType::REINHARD},
        gamma{2.2f},
        textureIn{resolution, GL_RGBA32F, GL_RGBA, GL_FLOAT},
        toneMapping(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                    "shaders" / "tone-mapping.comp"),
        vertexShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "shaders" / "render.vert"),
        fragmentShader(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                       "shaders" / "render.frag") {
    textureIn.loadHDR(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "img" /
                      "PaperMill_E_3k.hdr");
    textureOut.loadHDR(std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) / "img" /
                       "PaperMill_E_3k.hdr");

    toneMappingPipeline.attachComputeShader(toneMapping);

    renderPipeline.attachVertexShader(vertexShader);
    renderPipeline.attachFragmentShader(fragmentShader);
  }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  float getExposure() const { return exposure; }
  void setExposure(float exposure) { this->exposure = exposure; }

  bool getToneMappingOnRGB() const { return toneMappingOnRGB; }
  void setToneMappingOnRGB(bool toneMappingOnRGB) {
    this->toneMappingOnRGB = toneMappingOnRGB;
  }

  ToneMappingType getToneMappingType() const { return toneMappingType; }
  void setToneMappingType(const ToneMappingType& type) {
    this->toneMappingType = type;
  }

  float getGamma() const { return gamma; }
  void setGamma(float gamma) { this->gamma = gamma; }

  void render() const {
    // run compute shader
    textureIn.bindToImageUnit(0, GL_READ_ONLY);
    textureOut.bindToImageUnit(1, GL_WRITE_ONLY);
    toneMapping.setUniform("exposure", exposure);
    toneMapping.setUniform("toneMappingOnRGB", toneMappingOnRGB);
    toneMapping.setUniform("toneMappingType",
                           static_cast<int>(toneMappingType));
    toneMapping.setUniform("gamma", gamma);
    toneMappingPipeline.activate();
    const glm::uvec2 image_resolution = textureIn.getResolution();
    glDispatchCompute(std::ceil(image_resolution.x / 8.0f),
                      std::ceil(image_resolution.y / 8.0f), 1);
    toneMappingPipeline.deactivate();

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // render quad
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    textureOut.bindToTextureUnit(0);
    quad.draw(renderPipeline);
  }
};

#endif