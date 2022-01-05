#ifndef _CSS_QUAD_H
#define _CSS_QUAD_H
#include <vector>

#include "glad/gl.h"
//
#include "buffer.h"
#include "shader.h"

namespace gcss {

class Quad {
 private:
  GLuint VAO;
  Buffer VBO;
  Buffer EBO;

 public:
  Quad() {
    // setup VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // setup VBO
    // position and texcoords
    const std::vector<GLfloat> vertices = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f};
    VBO.setData(vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO.getName());

    // setup EBO
    const std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
    EBO.setData(indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO.getName());

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (GLvoid*)0);

    // texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (GLvoid*)(3 * sizeof(float)));

    // unbind VAO, VBO, EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    spdlog::info("[Quad] create VAO {:x}", VAO);
  }

  Quad(const Quad& other) = delete;

  Quad(Quad&& other) : VAO(other.VAO) { other.VAO = 0; }

  ~Quad() { release(); }

  Quad& operator=(const Quad& other) = delete;

  Quad& operator=(Quad&& other) {
    if (this != &other) {
      release();

      VAO = other.VAO;

      other.VAO = 0;
    }

    return *this;
  }

  void release() {
    if (VAO) {
      spdlog::info("[Quad] release VAO {:x}", VAO);
      glDeleteVertexArrays(1, &VAO);
    }
  }

  void draw(const Shader& shader) const {
    shader.activate();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader.deactivate();
  }
};

}  // namespace gcss

#endif