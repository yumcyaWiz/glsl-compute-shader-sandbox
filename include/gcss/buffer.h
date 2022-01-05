#ifndef _GCSS_BUFFER_H
#define _GCSS_BUFFER_H
#include <vector>

#include "glad/gl.h"
#include "spdlog/spdlog.h"

namespace gcss {

class Buffer {
 private:
  GLuint buffer;
  uint32_t size;

 public:
  Buffer() : buffer{0}, size{0} {
    glCreateBuffers(1, &buffer);

    spdlog::info("[Buffer] created buffer {:x}", buffer);
  }

  Buffer(const Buffer& buffer) = delete;

  Buffer(Buffer&& other) : buffer(other.buffer), size(other.size) {
    other.buffer = 0;
  }

  ~Buffer() { release(); }

  Buffer& operator=(const Buffer& buffer) = delete;

  Buffer& operator=(Buffer&& other) {
    if (this != &other) {
      release();

      buffer = other.buffer;
      size = other.size;

      other.buffer = 0;
    }

    return *this;
  }

  void release() {
    if (buffer) {
      spdlog::info("[Buffer] release buffer {:x}", buffer);

      glDeleteBuffers(1, &buffer);
      this->buffer = 0;
    }
  }

  GLuint getName() const { return buffer; }

  uint32_t getLength() const { return size; }

  template <typename T>
  void setData(const std::vector<T>& data, GLenum usage) {
    glNamedBufferData(this->buffer, sizeof(T) * data.size(), data.data(),
                      usage);
    this->size = data.size();
  }

  void bindToShaderStorageBuffer(GLuint binding_point_index) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, buffer);
  }
};

}  // namespace gcss

#endif