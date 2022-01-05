#ifndef _CSS_SHADER_H
#define _CSS_SHADER_H
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
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint computeShader;

  GLuint program;

  static void checkShaderCompilation(GLuint shader) {
    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
      spdlog::error("[Shader] failed to compile shader");

      GLint logSize = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetShaderInfoLog(shader, logSize, &logSize, errorLog.data());
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      spdlog::error("[Shader] {}", errorLogStr);
    }
  }

  static std::string loadStringFromFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
      spdlog::error("[Shader] failed to open {}", filepath.generic_string());
      std::exit(EXIT_FAILURE);
    }
    return std::string(std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>());
  }

  void destroyVertexShader() {
    spdlog::info("[Shader] vertex shader {:x} deleted", this->vertexShader);

    glDeleteShader(this->vertexShader);
    this->vertexShader = 0;
  }
  void destroyFragmentShader() {
    spdlog::info("[Shader] fragment shader {:x} deleted", this->fragmentShader);

    glDeleteShader(this->fragmentShader);
    this->fragmentShader = 0;
  }
  void destroyComputeShader() {
    spdlog::info("[Shader] compute shader {:x} deleted", this->computeShader);

    glDeleteShader(this->computeShader);
    this->computeShader = 0;
  }

  void destroyProgram() {
    spdlog::info("[Shader] program {:x} deleted", this->program);

    glDeleteProgram(this->program);
    this->program = 0;
  }

 public:
  Shader() : vertexShader{0}, fragmentShader{0}, computeShader{0}, program{0} {}

  virtual ~Shader() { release(); }

  Shader(const Shader& other) = delete;

  Shader(Shader&& other)
      : vertexShader{other.vertexShader},
        fragmentShader{other.fragmentShader},
        computeShader{other.computeShader},
        program{other.program} {
    other.vertexShader = 0;
    other.fragmentShader = 0;
    other.computeShader = 0;
    other.program = 0;
  }

  Shader& operator=(const Shader& other) = delete;

  Shader& operator=(Shader&& other) {
    if (this != &other) {
      release();

      vertexShader = other.vertexShader;
      fragmentShader = other.fragmentShader;
      computeShader = other.computeShader;
      program = other.program;

      other.vertexShader = 0;
      other.fragmentShader = 0;
      other.computeShader = 0;
      other.program = 0;
    }

    return *this;
  }

  void setVertexShader(const std::filesystem::path& vertexShaderFilepath) {
    // delete previous shader
    if (this->vertexShader) {
      this->destroyVertexShader();
    }

    spdlog::info("[Shader] loading vertex shader from {}",
                 vertexShaderFilepath.generic_string());

    // compile compute shader
    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const std::string vertex_shader_source =
        this->loadStringFromFile(vertexShaderFilepath);
    const char* vertex_shader_source_c = vertex_shader_source.c_str();
    glShaderSource(this->vertexShader, 1, &vertex_shader_source_c, nullptr);
    glCompileShader(this->vertexShader);
    this->checkShaderCompilation(this->vertexShader);

    spdlog::info("[Shader] vertex shader {:x} created", this->vertexShader);
  }

  void setFragmentShader(const std::filesystem::path& fragmentShaderFilepath) {
    // delete previous shader
    if (this->fragmentShader) {
      this->destroyFragmentShader();
    }

    spdlog::info("[Shader] loading fragment shader from {}",
                 fragmentShaderFilepath.generic_string());

    // compile compute shader
    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const std::string fragment_shader_source =
        this->loadStringFromFile(fragmentShaderFilepath);
    const char* fragment_shader_source_c = fragment_shader_source.c_str();
    glShaderSource(this->fragmentShader, 1, &fragment_shader_source_c, nullptr);
    glCompileShader(this->fragmentShader);
    this->checkShaderCompilation(this->fragmentShader);

    spdlog::info("[Shader] compute shader {:x} created", this->fragmentShader);
  }

  void setComputeShader(const std::filesystem::path& computeShaderFilepath) {
    // delete previous shader
    if (this->computeShader) {
      this->destroyComputeShader();
    }

    spdlog::info("[Shader] loading compute shader from {}",
                 computeShaderFilepath.generic_string());

    // compile compute shader
    this->computeShader = glCreateShader(GL_COMPUTE_SHADER);
    const std::string compute_shader_source =
        this->loadStringFromFile(computeShaderFilepath);
    const char* compute_shader_source_c = compute_shader_source.c_str();
    glShaderSource(this->computeShader, 1, &compute_shader_source_c, nullptr);
    glCompileShader(this->computeShader);
    this->checkShaderCompilation(this->computeShader);

    spdlog::info("[Shader] compute shader {:x} created", this->computeShader);
  }

  // compile and link shaders
  void linkShader() {
    // link shader program
    this->program = glCreateProgram();
    if (this->vertexShader) {
      glAttachShader(this->program, this->vertexShader);
    }
    if (this->fragmentShader) {
      glAttachShader(this->program, this->fragmentShader);
    }
    if (this->computeShader) {
      glAttachShader(this->program, this->computeShader);
    }

    glLinkProgram(this->program);

    if (this->vertexShader) {
      glDetachShader(this->program, this->vertexShader);
    }
    if (this->fragmentShader) {
      glDetachShader(this->program, this->fragmentShader);
    }
    if (this->computeShader) {
      glDetachShader(this->program, this->computeShader);
    }

    // handle link error
    int success = 0;
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
      spdlog::error("[Shader] failed to link shaders");

      GLint logSize = 0;
      glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &logSize);
      std::vector<GLchar> errorLog(logSize);
      glGetProgramInfoLog(this->program, logSize, &logSize, &errorLog[0]);
      std::string errorLogStr(errorLog.begin(), errorLog.end());
      spdlog::error("[Shader] {}", errorLogStr);

      glDeleteProgram(this->program);
    }

    spdlog::info("[Shader] program {:x} created", this->program);
  }

  void release() {
    if (this->vertexShader) {
      spdlog::info("[Shader] release shader {:x}", this->vertexShader);
      this->destroyVertexShader();
    }

    if (this->fragmentShader) {
      spdlog::info("[Shader] release shader {:x}", this->fragmentShader);
      this->destroyFragmentShader();
    }

    if (this->computeShader) {
      spdlog::info("[Shader] release shader {:x}", this->computeShader);
      this->destroyComputeShader();
    }

    if (this->program) {
      spdlog::info("[Shader] release program {:x}", this->program);
      this->destroyProgram();
    }
  }

  // activate shader on the current context
  void activate() const { glUseProgram(this->program); }

  // deactivate shader on the current context
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

class ComputeShader : public Shader {
 public:
  ComputeShader() : Shader() {}

  void run(GLuint work_groups_x, GLuint work_groups_y,
           GLuint work_groups_z) const {
    this->activate();
    glDispatchCompute(work_groups_x, work_groups_y, work_groups_z);
    this->deactivate();

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT);
  }
};

}  // namespace gcss

#endif