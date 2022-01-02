#ifndef _PARTICLES_H
#define _PARTICLES_H
#include "gcss/buffer.h"
#include "gcss/shader.h"

using namespace gcss;

struct alignas(16) Particle {
  glm::vec4 position = glm::vec4(0);
  glm::vec4 velocity = glm::vec4(0);
  glm::vec4 force = glm::vec4(0);
  float mass = 0;
};

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          (GLvoid*)0);

    // velocity
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          (GLvoid*)(4 * sizeof(GLfloat)));

    // force
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          (GLvoid*)(8 * sizeof(GLfloat)));

    // mass
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          (GLvoid*)(12 * sizeof(GLfloat)));

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
    glBindVertexArray(0);

    shader.deactivate();
  }
};

#endif