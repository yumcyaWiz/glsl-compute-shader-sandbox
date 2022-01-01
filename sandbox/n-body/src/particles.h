#ifndef _PARTICLES_H
#define _PARTICLES_H
#include "gcss/buffer.h"
#include "gcss/shader.h"

using namespace gcss;

class Particles {
 private:
  GLuint VAO;
  const Buffer* positions;

 public:
  Particles(const Buffer* positions) : positions{positions} { init(); }

  void init() {
    // setup VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, positions->getName());

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (GLvoid*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void destroy() {
    glDeleteVertexArrays(1, &VAO);
    this->VAO = 0;
  }

  void draw(const Shader& shader) const {
    shader.activate();

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, positions->getLength());

    shader.deactivate();
  }
};

#endif