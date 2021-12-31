#ifndef _GCSS_BUFFER_H
#define _GCSS_BUFFER_H
#include <vector>

#include "glad/gl.h"

namespace gcss {

class Buffer {
 protected:
  GLuint buffer;

 public:
  Buffer() { glGenBuffers(1, &buffer); }

  void destroy() { glDeleteBuffers(1, &buffer); }
};

class ShaderStorageBuffer : public Buffer {
 public:
  ShaderStorageBuffer() : Buffer() {}

  template <typename T>
  void setData(const std::vector<T>& data) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), data.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  void bind(GLuint binding_point_index) const {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, buffer);
  }
};

}  // namespace gcss

#endif