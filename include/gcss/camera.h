#ifndef _GCSS_CAMERA_H
#define _GCSS_CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace gcss {

enum class CameraMovement {
  FORWARD,
  BACKWARD,
  RIGHT,
  LEFT,
  UP,
  DOWN,
};

class Camera {
 private:
  glm::vec3 camPos;
  glm::vec3 camForward;
  glm::vec3 camRight;
  glm::vec3 camUp;

  float fov;
  float znear;
  float zfar;

  float movementSpeed;
  float lookAroundSpeed;

  float phi;
  float theta;

 public:
  Camera()
      : camPos{0.0f, 0.0f, 1.0f},
        camForward{0.0f, 0.0f, -1.0f},
        camRight{1.0f, 0.0f, 0.0f},
        camUp{0.0f, 1.0f, 0.0f},
        fov{45.0f},
        znear{0.1f},
        zfar{10000.0f},
        movementSpeed{1.0f},
        lookAroundSpeed{0.1f},
        phi{270.0f},
        theta{90.0f} {}

  glm::mat4 computeViewMatrix() const {
    return glm::lookAt(camPos, camPos + camForward, camUp);
  }

  glm::mat4 computeProjectionMatrix(uint32_t width, uint32_t height) const {
    return glm::perspective(glm::radians(fov),
                            static_cast<float>(width) / height, znear, zfar);
  }

  glm::mat4 computeViewProjectionmatrix(uint32_t width, uint32_t height) const {
    return computeProjectionMatrix(width, height) * computeViewMatrix();
  }

  void reset() { *this = Camera(); }

  void move(const CameraMovement& direction, float dt) {
    const float velocity = movementSpeed * dt;
    switch (direction) {
      case CameraMovement::FORWARD:
        camPos += velocity * camForward;
        break;
      case CameraMovement::BACKWARD:
        camPos -= velocity * camForward;
        break;
      case CameraMovement::RIGHT:
        camPos += velocity * camRight;
        break;
      case CameraMovement::LEFT:
        camPos -= velocity * camRight;
        break;
      case CameraMovement::UP:
        camPos += velocity * camUp;
        break;
      case CameraMovement::DOWN:
        camPos -= velocity * camUp;
        break;
    }
  }

  void lookAround(float d_phi, float d_theta) {
    // update phi, theta
    phi += lookAroundSpeed * d_phi;
    if (phi < 0.0f) phi = 360.0f;
    if (phi > 360.0f) phi = 0.0f;

    theta += lookAroundSpeed * d_theta;
    if (theta < 0.0f) theta = 180.0f;
    if (theta > 180.0f) theta = 0.0f;

    // set camera directions
    const float phiRad = glm::radians(phi);
    const float thetaRad = glm::radians(theta);
    camForward =
        glm::vec3(std::cos(phiRad) * std::sin(thetaRad), std::cos(thetaRad),
                  std::sin(phiRad) * std::sin(thetaRad));
    camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1.0f, 0)));
    camUp = glm::normalize(glm::cross(camRight, camForward));
  }

  void getRay(const glm::vec2& window, const glm::uvec2& resolution,
              glm::vec3& origin, glm::vec3& direction) const {
    const glm::mat4 view = computeViewMatrix();
    const glm::mat4 projection =
        computeProjectionMatrix(resolution.x, resolution.y);
    const glm::vec4 viewport = glm::vec4(0, 0, resolution.x, resolution.y);

    const glm::vec3 p1 =
        glm::unProject(glm::vec3(window, 0), view, projection, viewport);
    const glm::vec3 p2 =
        glm::unProject(glm::vec3(window, 1), view, projection, viewport);

    origin = p1;
    direction = glm::normalize(p2 - p1);
  }
};

}  // namespace gcss

#endif