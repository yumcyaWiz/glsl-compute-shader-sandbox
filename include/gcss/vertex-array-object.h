#ifndef _GCSS_VERTEX_ARRAY_OBJECT
#define _GCSS_VERTEX_ARRAY_OBJECT

#include "glad/gl.h"
#include "spdlog/spdlog.h"
//
#include "buffer.h"

namespace gcss {

class VertexArrayObject {
 private:
  GLuint array;

 public:
  VertexArrayObject() { glCreateVertexArrays(1, &array); }

  VertexArrayObject(const VertexArrayObject& other) = delete;

  VertexArrayObject(VertexArrayObject&& other) : array(other.array) {
    other.array = 0;
  }

  ~VertexArrayObject() { release(); }

  VertexArrayObject& operator=(const VertexArrayObject& other) = delete;

  VertexArrayObject& operator=(VertexArrayObject&& other) {
    if (this != &other) {
      release();

      array = other.array;

      other.array = 0;
    }

    return *this;
  }

  void bindVertexBuffer(const Buffer& buffer, GLuint binding, GLintptr offset,
                        GLsizei stride) const {
    glVertexArrayVertexBuffer(array, binding, buffer.getName(), offset, stride);
  }

  void bindElementBuffer(const Buffer& buffer) const {
    glVertexArrayElementBuffer(array, buffer.getName());
  }

  void activateVertexAttribution(GLuint binding, GLuint attrib, GLint size,
                                 GLenum type, GLsizei offset) const {
    glEnableVertexArrayAttrib(array, attrib);
    glVertexArrayAttribBinding(array, attrib, binding);
    glVertexArrayAttribFormat(array, attrib, size, type, GL_FALSE, offset);
  }

  void activate() const { glBindVertexArray(array); }

  void deactivate() const { glBindVertexArray(0); }

  void release() {
    if (array) {
      spdlog::info("[VertexArrayObject] release VAO {:x}", array);
      glDeleteVertexArrays(1, &array);
    }
  }
};

}  // namespace gcss

#endif