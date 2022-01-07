#ifndef _PARTICLES_H
#define _PARTICLES_H
#include "gcss/buffer.h"
#include "gcss/shader.h"
#include "gcss/vertex-array-object.h"
#include "glm/glm.hpp"

using namespace gcss;

struct alignas(16) Particle {
  glm::vec4 position = glm::vec4(0);
  glm::vec4 velocity = glm::vec4(0);
  float mass = 0;
};

class Particles {
 private:
  VertexArrayObject VAO;
  const Buffer* buffer;

 public:
  Particles() {}

  void setParticles(const Buffer* buffer) {
    this->buffer = buffer;
    VAO.bindVertexBuffer(*buffer, 0, 0, sizeof(Particle));

    // position
    VAO.activateVertexAttribution(0, 0, 3, GL_FLOAT, 0);

    // velocity
    VAO.activateVertexAttribution(0, 1, 3, GL_FLOAT, 4 * sizeof(GLfloat));

    // mass
    VAO.activateVertexAttribution(0, 2, 1, GL_FLOAT, 8 * sizeof(GLfloat));
  }

  void draw(const Pipeline& pipeline) const {
    pipeline.activate();
    VAO.activate();
    glDrawArrays(GL_POINTS, 0, buffer->getLength());
    VAO.deactivate();
    pipeline.deactivate();
  }
};

#endif