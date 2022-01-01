#ifndef _PARTICLES_H
#define _PARTICLES_H
#include "gcss/buffer.h"
#include "gcss/shader.h"

using namespace gcss;

class Particles {
 private:
  GLuint VAO;
  const Buffer* particles;

 public:
  Particles(const Buffer* particles) : particles{particles} { init(); }

  void init() {
    // setup VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, particles->getName());

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)0);

    // velocity
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (GLvoid*)(4 * sizeof(GLfloat)));

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
    glDrawArrays(GL_POINTS, 0, particles->getLength());

    shader.deactivate();
  }
};

#endif