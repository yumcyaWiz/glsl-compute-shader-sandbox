#ifndef _CSS_TEXTURE_H
#define _CSS_TEXTURE_H
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"

namespace gcss {

class Texture {
 private:
  glm::uvec2 resolution;
  GLuint texture;
  GLint internalFormat;
  GLenum format;
  GLenum type;

 public:
  Texture() {}

  Texture(const glm::uvec2& resolution, GLint internal_format, GLenum format,
          GLenum type)
      : resolution(resolution),
        internalFormat(internal_format),
        format(format),
        type(type) {
    // init texture
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->resolution.x,
                 this->resolution.y, 0, this->format, this->type, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    spdlog::info("[Texture] texture {:x} created", this->texture);
  }

  GLuint getTextureName() const { return this->texture; }

  GLint getInternalFormat() const { return this->internalFormat; }

  GLenum getFormat() const { return this->format; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;

    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->resolution.x,
                 this->resolution.y, 0, this->format, this->type, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  template <typename T>
  void setImage(const std::vector<T>& image) const {
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, this->resolution.x,
                 this->resolution.y, 0, this->format, this->type, image.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // bind texture to the specified texture unit
  void bindToTextureUnit(GLuint texture_unit_number) const {
    glActiveTexture(GL_TEXTURE0 + texture_unit_number);
    glBindTexture(GL_TEXTURE_2D, this->texture);
  }

  // bind texture to the specified image unit
  void bindToImageUnit(GLuint image_unit_number, GLenum access) const {
    glActiveTexture(GL_TEXTURE0 + image_unit_number);
    glBindImageTexture(image_unit_number, this->texture, 0, GL_FALSE, 0, access,
                       this->internalFormat);
  }

  // destroy texture object
  void destroy() {
    spdlog::info("[Texture] texture {:x} deleted", this->texture);

    glDeleteTextures(1, &this->texture);
    this->texture = 0;
  }
};

}  // namespace gcss

#endif