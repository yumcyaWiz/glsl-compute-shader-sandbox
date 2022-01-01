#ifndef _GCSS_BUFFER_H
#define _GCSS_BUFFER_H
#include <vector>

#include "glad/gl.h"
#include "spdlog/spdlog.h"

namespace gcss {

class Buffer {
 protected:
  GLuint buffer;

 public:
  Buffer() {
    spdlog::info("[Buffer] create buffer");

    glGenBuffers(1, &buffer);
  }

  void destroy() {
    spdlog::info("[Buffer] destroy buffer");

    glDeleteBuffers(1, &buffer);
    this->buffer = 0;
  }

  template <typename T>
  void setData(const std::vector<T>& data, GLenum usage) const {
    glNamedBufferData(buffer, sizeof(T) * data.size(), data.data(), usage);
  }

  void bindToShaderStorageBuffer(GLuint binding_point_index) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, buffer);
  }
};

}  // namespace gcss

#endif