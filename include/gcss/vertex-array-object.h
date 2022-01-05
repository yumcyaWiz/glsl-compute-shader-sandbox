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

  void bindBuffer(GLenum target, const Buffer& buffer) const {
    activate();
    glBindBuffer(target, buffer.getName());
    deactivate();
  }

  void activateVertexAttribution(GLuint index, GLint size, GLenum type,
                                 GLsizei stride, GLsizei offset) const {
    activate();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (GLvoid*)offset);
    deactivate();
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