#ifndef _CSS_TEXTURE_H
#define _CSS_TEXTURE_H
#include "glad/gl.h"

namespace css {

class Texture {
 private:
  uint32_t width;
  uint32_t height;
  GLuint texture;

  Texture(uint32_t width, uint32_t height) : width(width), height(height) {
    // init texture
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // destroy texture object
  void destroy() {
    glDeleteTextures(1, &this->texture);
    this->texture = 0;
  }
};

}  // namespace css

#endif