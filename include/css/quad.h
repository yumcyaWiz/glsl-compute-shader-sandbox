#ifndef _CSS_QUAD_H
#define _CSS_QUAD_H

#include "glad/gl.h"
#include "shader.h"

namespace css {

class Quad {
 private:
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

 public:
  Quad() {
    // setup VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // setup VBO
    // position and texcoords
    constexpr GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f,
                                    0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
                                    1.0f,  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // setup EBO
    constexpr GLuint indices[] = {0, 1, 2, 2, 3, 0};
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

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
  }

  void destroy() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
  }

  void draw(const Shader& shader) const {
    shader.activate();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader.deactivate();
  }
};

}  // namespace css

#endif