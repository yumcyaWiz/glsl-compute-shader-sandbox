#ifndef _CSS_TEXTURE_H
#define _CSS_TEXTURE_H
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gcss {
class Texture {
 private:
  glm::uvec2 resolution;
  GLuint texture;
  GLint internalFormat;
  GLenum format;
  GLenum type;

 public:
  Texture() {
    // init texture
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    spdlog::info("[Texture] texture {:x} created", this->texture);
  }

  Texture(const glm::uvec2& resolution, GLint internalFormat, GLenum format,
          GLenum type)
      : Texture() {
    initImage(resolution, internalFormat, format, type);
  }

  Texture(const Texture& other) = delete;

  Texture(Texture&& other)
      : resolution(other.resolution),
        texture(other.texture),
        internalFormat(other.internalFormat),
        format(other.format),
        type(other.type) {
    other.texture = 0;
  }

  ~Texture() { release(); }

  Texture& operator=(const Texture& other) = delete;

  Texture& operator=(Texture&& other) {
    if (this != &other) {
      release();

      resolution = std::move(other.resolution);
      texture = other.texture;
      internalFormat = other.internalFormat;
      format = other.format;
      type = other.type;

      other.texture = 0;
    }

    return *this;
  }

  glm::uvec2 getResolution() const { return this->resolution; }

  GLuint getTextureName() const { return this->texture; }

  GLint getInternalFormat() const { return this->internalFormat; }

  GLenum getFormat() const { return this->format; }

  GLenum getType() const { return this->type; }

  void initImage(const glm::uvec2& resolution, GLint internalFormat,
                 GLenum format, GLenum type) {
    this->resolution = resolution;
    this->internalFormat = internalFormat;
    this->format = format;
    this->type = type;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resolution.x, resolution.y,
                 0, format, type, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  template <typename T>
  void setImage(const std::vector<T>& image, const glm::uvec2& resolution,
                GLint internalFormat, GLenum format, GLenum type) {
    this->resolution = resolution;
    this->internalFormat = internalFormat;
    this->format = format;
    this->type = type;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resolution.x, resolution.y,
                 0, format, type, image.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  template <typename T>
  void setImage(const T* image, const glm::uvec2& resolution,
                GLint internalFormat, GLenum format, GLenum type) {
    this->resolution = resolution;
    this->internalFormat = internalFormat;
    this->format = format;
    this->type = type;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, resolution.x, resolution.y,
                 0, format, type, image);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void resize(const glm::uvec2& resolution) {
    initImage(resolution, internalFormat, format, type);
  }

  void loadHDR(const std::filesystem::path& filepath) {
    const std::string filepath_str = filepath.generic_string();
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    float* image =
        stbi_loadf(filepath_str.c_str(), &width, &height, &channels, 4);
    if (!image) {
      spdlog::error("[Texture] failed to load {}", filepath_str);
    }

    this->internalFormat = GL_RGBA32F;
    this->format = GL_RGBA;
    this->type = GL_FLOAT;
    setImage(image, glm::uvec2(width, height), GL_RGBA32F, GL_RGBA, GL_FLOAT);

    stbi_image_free(image);
  }

  // bind texture to the specified texture unit
  void bindToTextureUnit(GLuint texture_unit_number) const {
    glBindTextureUnit(texture_unit_number, texture);
  }

  // bind texture to the specified image unit
  void bindToImageUnit(GLuint image_unit_number, GLenum access) const {
    glBindImageTexture(image_unit_number, this->texture, 0, GL_FALSE, 0, access,
                       this->internalFormat);
  }

  void release() {
    if (texture) {
      spdlog::info("[Texture] release texture {:x}", this->texture);

      glDeleteTextures(1, &this->texture);
      this->texture = 0;
    }
  }
};

}  // namespace gcss

#endif