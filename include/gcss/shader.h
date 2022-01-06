#ifndef _GCSS_SHADER_H
#define _GCSS_SHADER_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "spdlog/spdlog.h"
//
#include "texture.h"

namespace gcss {

class Shader {
 private:
  GLuint program;

  static std::string loadStringFromFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      spdlog::error("[Shader] failed to open {}", filepath.generic_string());
      std::exit(EXIT_FAILURE);
    }
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  }

 public:
  Shader(GLenum type, const std::filesystem::path& filepath) {
    const std::string shader_source = loadStringFromFile(filepath);
    const char* shader_source_c = shader_source.c_str();
    program = glCreateShaderProgramv(type, 1, &shader_source_c);
    spdlog::info("[Shader] program {:x} created", program);

    // check compile and link error
    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
      spdlog::error("[Shader] failed to link program {:x}", program);

      GLint logSize = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      spdlog::error("[Shader] {}", errorLogStr);
    }
  }

  virtual ~Shader() { release(); }

  Shader(const Shader& other) = delete;

  Shader(Shader&& other) : program(other.program) { other.program = 0; }

  Shader& operator=(const Shader& other) = delete;

  Shader& operator=(Shader&& other) {
    if (this != &other) {
      release();

      program = other.program;

      other.program = 0;
    }

    return *this;
  }

  void release() {
    if (program) {
      spdlog::info("[Shader] release program {:x}", program);
      glDeleteProgram(program);
    }
  }

  GLuint getProgram() const { return program; }

  void setUniform(const std::string& uniform_name,
                  const std::variant<bool, GLint, GLuint, GLfloat, glm::vec2,
                                     glm::vec3, glm::mat4>& value) const {
    // get location of uniform variable
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());

    // set value
    struct Visitor {
      GLuint program;
      GLint location;
      Visitor(GLuint program, GLint location)
          : program(program), location(location) {}

      void operator()(bool value) {
        glProgramUniform1i(program, location, value);
      }
      void operator()(GLint value) {
        glProgramUniform1i(program, location, value);
      }
      void operator()(GLuint value) {
        glProgramUniform1ui(program, location, value);
      }
      void operator()(GLfloat value) {
        glProgramUniform1f(program, location, value);
      }
      void operator()(const glm::vec2& value) {
        glProgramUniform2fv(program, location, 1, glm::value_ptr(value));
      }
      void operator()(const glm::vec3& value) {
        glProgramUniform3fv(program, location, 1, glm::value_ptr(value));
      }
      void operator()(const glm::mat4& value) {
        glProgramUniformMatrix4fv(program, location, 1, GL_FALSE,
                                  glm::value_ptr(value));
      }
    };
    std::visit(Visitor{program, location}, value);
  }
};

class VertexShader : public Shader {
 public:
  VertexShader(const std::filesystem::path& filepath)
      : Shader(GL_VERTEX_SHADER, filepath) {}
};

class GeometryShader : public Shader {
 public:
  GeometryShader(const std::filesystem::path& filepath)
      : Shader(GL_GEOMETRY_SHADER, filepath) {}
};

class FragmentShader : public Shader {
 public:
  FragmentShader(const std::filesystem::path& filepath)
      : Shader(GL_FRAGMENT_SHADER, filepath) {}
};

class ComputeShader : public Shader {
 public:
  ComputeShader(const std::filesystem::path& filepath)
      : Shader(GL_COMPUTE_SHADER, filepath) {}
};

class Pipeline {
 public:
  GLuint pipeline;

  Pipeline() {
    glCreateProgramPipelines(1, &pipeline);
    spdlog::info("[Pipeline] pipeline {:x} created", pipeline);
  }

  Pipeline(const Pipeline& other) = delete;

  Pipeline(Pipeline&& other) : pipeline(other.pipeline) { other.pipeline = 0; }

  ~Pipeline() { release(); }

  Pipeline& operator=(const Pipeline& other) = delete;

  Pipeline& operator=(Pipeline&& other) {
    if (this != &other) {
      release();

      pipeline = other.pipeline;

      other.pipeline = 0;
    }

    return *this;
  }

  void release() {
    if (pipeline) {
      spdlog::info("[Pipeline] release pipeline {:x}", pipeline);
      glDeleteProgramPipelines(1, &pipeline);
    }
  }

  void attachVertexShader(const VertexShader& shader) const {
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, shader.getProgram());
  }

  void attachFragmentShader(const FragmentShader& shader) const {
    glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, shader.getProgram());
  }

  void attachComputeShader(const ComputeShader& shader) const {
    glUseProgramStages(pipeline, GL_COMPUTE_SHADER_BIT, shader.getProgram());
  }

  void activate() const { glBindProgramPipeline(pipeline); }

  void deactivate() const { glBindProgramPipeline(0); }
};

}  // namespace gcss

#endif