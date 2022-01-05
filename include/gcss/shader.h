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
  GLuint shader;
  GLenum type;

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
  Shader(GLenum type) {
    shader = glCreateShader(type);

    switch (type) {
      case GL_VERTEX_SHADER:
        spdlog::info("[Shader] vertex shader {:x} created", shader);
        break;
      case GL_GEOMETRY_SHADER:
        spdlog::info("[Shader] geometry shader {:x} created", shader);
        break;
      case GL_FRAGMENT_SHADER:
        spdlog::info("[Shader] fragment shader {:x} created", shader);
        break;
      case GL_COMPUTE_SHADER:
        spdlog::info("[Shader] compute shader {:x} created", shader);
        break;
    }
  }

  virtual ~Shader() { release(); }

  Shader(const Shader& other) = delete;

  Shader(Shader&& other) : shader(other.shader), type(other.type) {
    other.shader = 0;
  }

  Shader& operator=(const Shader& other) = delete;

  Shader& operator=(Shader&& other) {
    if (this != &other) {
      release();

      shader = other.shader;
      type = other.type;

      other.shader = 0;
    }

    return *this;
  }

  void release() {
    if (this->shader) {
      spdlog::info("[Shader] release shader {:x}", this->shader);
      glDeleteShader(shader);
    }
  }

  GLuint getName() const { return shader; }

  void compile(const std::filesystem::path& filepath) {
    spdlog::info("[Shader] loading shader from {}", filepath.generic_string());

    const std::string shader_source = loadStringFromFile(filepath);
    const char* shader_source_c = shader_source.c_str();
    glShaderSource(shader, 1, &shader_source_c, nullptr);

    glCompileShader(shader);

    // show compile error
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
      spdlog::error("[Shader] failed to compile shader {:x}", shader);

      GLint logSize = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetShaderInfoLog(shader, logSize, &logSize, errorLog.data());
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      spdlog::error("[Shader] {}", errorLogStr);
    }
  }
};

class VertexShader : public Shader {
 public:
  VertexShader() : Shader(GL_VERTEX_SHADER) {}
};

class GeometryShader : public Shader {
 public:
  GeometryShader() : Shader(GL_GEOMETRY_SHADER) {}
};

class FragmentShader : public Shader {
 public:
  FragmentShader() : Shader(GL_FRAGMENT_SHADER) {}
};

class ComputeShader : public Shader {
 public:
  ComputeShader() : Shader(GL_COMPUTE_SHADER) {}
};

class Program {
 public:
  GLuint program;

  Program() {
    program = glCreateProgram();
    spdlog::info("[Program] program {:x} created", program);
  }

  Program(const Program& other) = delete;

  Program(Program&& other) : program(other.program) { other.program = 0; }

  ~Program() { release(); }

  Program& operator=(const Program& other) = delete;

  Program& operator=(Program&& other) {
    if (this != &other) {
      release();

      program = other.program;

      other.program = 0;
    }

    return *this;
  }

  void release() {
    if (program) {
      spdlog::info("[Program] release program {:x}", program);
      glDeleteProgram(program);
    }
  }

  void attachShader(const Shader& shader) const {
    glAttachShader(program, shader.getName());
  }

  void detachShader(const Shader& shader) const {
    glDetachShader(program, shader.getName());
  }

  void linkProgram() const {
    glLinkProgram(program);

    // show link error
    int success = 0;
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
      spdlog::error("[Program] failed to link shaders");

      GLint logSize = 0;
      glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetProgramInfoLog(this->program, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      spdlog::error("[Program] {}", errorLogStr);
    }
  }

  void activate() const { glUseProgram(program); }

  void deactivate() const { glUseProgram(0); }

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

}  // namespace gcss

#endif