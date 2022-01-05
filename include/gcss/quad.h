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
    VAO.bindBuffer(GL_ARRAY_BUFFER, VBO);

    // setup EBO
    const std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};
    EBO.setData(indices, GL_STATIC_DRAW);
    VAO.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // position
    VAO.activateVertexAttribution(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);

    // texcoords
    VAO.activateVertexAttribution(1, 2, GL_FLOAT, 5 * sizeof(GLfloat),
                                  3 * sizeof(GLfloat));
  }

  void draw(const Program& program) const {
    program.activate();
    VAO.activate();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    VAO.deactivate();
    program.deactivate();
  }
};

}  // namespace gcss

#endif