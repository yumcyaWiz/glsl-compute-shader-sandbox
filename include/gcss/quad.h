#ifndef _CSS_QUAD_H
#define _CSS_QUAD_H
#include <vector>

#include "glad/gl.h"
//
#include "buffer.h"
#include "shader.h"
#include "vertex-array-object.h"

namespace gcss {

class Quad {
 private:
  VertexArrayObject VAO;
  Buffer VBO;
  Buffer EBO;

 public:
  Quad() {
    // setup VBO
    // position and texcoords
    const std::vector<GLfloat> vertices = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f};
    VBO.setData(vertices, GL_STATIC_DRAW);

    // setup EBO
    const std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
    EBO.setData(indices, GL_STATIC_DRAW);

    // setup VAO
    VAO.bindVertexBuffer(VBO, 0, 0, 5 * sizeof(GLfloat));
    VAO.bindElementBuffer(EBO);

    // position
    VAO.activateVertexAttribution(0, 0, 3, GL_FLOAT, 0);

    // texcoords
    VAO.activateVertexAttribution(0, 1, 2, GL_FLOAT, 3 * sizeof(GLfloat));
  }

  void draw(const Pipeline& pipeline) const {
    pipeline.activate();
    VAO.activate();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    VAO.deactivate();
    pipeline.deactivate();
  }
};

}  // namespace gcss

#endif