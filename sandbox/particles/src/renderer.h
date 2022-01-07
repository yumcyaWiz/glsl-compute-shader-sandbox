#ifndef _RENDERER_H
#define _RENDERER_H
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/buffer.h"
#include "gcss/camera.h"
//
#include "particles.h"

using namespace gcss;

class Renderer {
 private:
  glm::uvec2 resolution;
  uint32_t nParticles;
  glm::vec3 gravityCenter;
  float gravityIntensity;
  float dt;

  Camera camera;

  Particles particles;
  Buffer particlesBuffer;

  ComputeShader updateParticles;
  Pipeline updateParticlesPipeline;

  VertexShader vertexShader;
  FragmentShader fragmentShader;
  Pipeline renderPipeline;

 public:
  Renderer()
      : resolution{512, 512},
        nParticles{1000000},
        gravityCenter{0},
        gravityIntensity{0.001f},
        dt{0.01f},
        updateParticles{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                        "shaders" / "update-particles.comp"},
        vertexShader{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                     "shaders" / "render-particles.vert"},
        fragmentShader{std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
                       "shaders" / "render-particles.frag"} {
    particles.setParticles(&particlesBuffer);

    updateParticlesPipeline.attachComputeShader(updateParticles);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    renderPipeline.attachVertexShader(vertexShader);
    renderPipeline.attachFragmentShader(fragmentShader);

    placeParticles();
  }

  glm::uvec2 getResolution() const { return this->resolution; }
  void setResolution(const glm::uvec2& resolution) {
    this->resolution = resolution;
  }

  uint32_t getNParticles() const { return nParticles; }
  void setNParticles(uint32_t nParticles) {
    this->nParticles = nParticles;
    placeParticles();
  }

  glm::vec3 getGravityCenter() const { return gravityCenter; }
  void setGravityCenter(const glm::vec3& gravityCenter) {
    this->gravityCenter = gravityCenter;
  }

  float getGravityIntensity() const { return gravityIntensity; }
  void setGravityIntensity(float gravityIntensity) {
    this->gravityIntensity = gravityIntensity;
  }

  float getDt() const { return dt; }
  void setDt(float dt) { this->dt = dt; }

  glm::vec3 screenToWorld(const glm::vec2& screen) const {
    glm::vec3 origin, direction;
    camera.getRay(screen, resolution, origin, direction);

    // ray-plane intersection at z = 0
    const float t = -origin.z / direction.z;
    glm::vec3 p = origin + t * direction;

    return p;
  }

  void placeParticles() {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_real_distribution<float> dist(-1, 1);

    std::vector<Particle> data(nParticles);
    for (std::size_t i = 0; i < nParticles; ++i) {
      data[i].position = glm::vec4(dist(mt), dist(mt), 0.5f * dist(mt), 0);
      data[i].velocity = glm::vec4(0);
      data[i].mass = 1.0f;
    }

    particlesBuffer.setData(data, GL_DYNAMIC_DRAW);
  }

  void move(const CameraMovement& movement_direction, float delta_time) {
    camera.move(movement_direction, delta_time);
  }

  void lookAround(float d_phi, float d_theta) {
    camera.lookAround(d_phi, d_theta);
  }

  void render() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // render particles
    vertexShader.setUniform(
        "viewProjection",
        camera.computeViewProjectionmatrix(resolution.x, resolution.y));
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, resolution.x, resolution.y);
    particles.draw(renderPipeline);

    // update particles
    particlesBuffer.bindToShaderStorageBuffer(0);
    updateParticles.setUniform("gravityCenter", gravityCenter);
    updateParticles.setUniform("gravityIntensity", gravityIntensity);
    updateParticles.setUniform("k", 0.01f);
    updateParticles.setUniform("dt", dt);
    updateParticlesPipeline.activate();
    glDispatchCompute(std::ceil(nParticles / 128.0f), 1, 1);
    updateParticlesPipeline.deactivate();
  }
};

#endif