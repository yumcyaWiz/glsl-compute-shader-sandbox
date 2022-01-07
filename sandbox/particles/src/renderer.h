#ifndef _RENDERER_H
#define _RENDERER_H
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/camera.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;
  Camera camera;

 public:
  Renderer() : resolution{512, 512} {}

  glm::uvec2 getResolution() const { return this->resolution; }

  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  void move(const CameraMovement& movement_direction, float delta_time) {
    camera.move(movement_direction, delta_time);
  }

  void lookAround(float d_phi, float d_theta) {
    camera.lookAround(d_phi, d_theta);
  }

  void render() {}
};

#endif