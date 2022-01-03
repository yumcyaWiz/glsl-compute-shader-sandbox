#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H
#include "glad/gl.h"
#include "spdlog/spdlog.h"
//
#include "texture.h"

namespace gcss {

class FrameBuffer {
 private:
  GLuint framebuffer;

 public:
  FrameBuffer() {
    glCreateFramebuffers(1, &framebuffer);

    spdlog::info("[FrameBuffer] framebuffer {:x} created", framebuffer);
  }

  void destroy() {
    spdlog::info("[FrameBuffer] framebuffer {:x} deleted", framebuffer);

    glDeleteFramebuffers(1, &framebuffer);
    this->framebuffer = 0;
  }

  void bindTexture(const Texture& texture, GLenum attachment) const {
    glNamedFramebufferTexture(framebuffer, attachment, texture.getTextureName(),
                              0);
  }

  void activate() const { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }

  void deactivate() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
};

}  // namespace gcss

#endif