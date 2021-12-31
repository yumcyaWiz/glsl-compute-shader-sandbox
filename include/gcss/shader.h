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
    glDeleteShader(this->vertexShader);
    this->vertexShader = 0;
  }
  void destroyFragmentShader() {
    glDeleteShader(this->fragmentShader);
    this->fragmentShader = 0;
  }
  void destroyComputeShader() {
    glDeleteShader(this->computeShader);
    this->computeShader = 0;
  }

  void destroyProgram() {
    glDeleteProgram(this->program);
    this->program = 0;
  }

 public:
  Shader() {}

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
  }

  // destroy shaders, program
  void destroy() {
    if (this->vertexShader) {
      this->destroyVertexShader();
    }
    if (this->fragmentShader) {
      this->destroyFragmentShader();
    }
    if (this->computeShader) {
      this->destroyComputeShader();
    }

    if (this->program) {
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
    activate();

    // get location of uniform variable
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());

    // set value
    struct Visitor {
      GLint location;
      Visitor(GLint location) : location(location) {}

      void operator()(bool value) { glUniform1i(location, value); }
      void operator()(GLint value) { glUniform1i(location, value); }
      void operator()(GLuint value) { glUniform1ui(location, value); }
      void operator()(GLfloat value) { glUniform1f(location, value); }
      void operator()(const glm::vec2& value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
      }
      void operator()(const glm::vec3& value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
      }
      void operator()(const glm::mat4& value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
      }
    };
    std::visit(Visitor{location}, value);

    deactivate();
  }

  void setTexture(const std::string& uniform_name, const Texture& texture,
                  GLuint texture_unit_number) const {
    activate();

    // bind texture to the specified texture unit
    glActiveTexture(GL_TEXTURE0 + texture_unit_number);
    glBindTexture(GL_TEXTURE_2D, texture.getTextureName());

    // set texture unit number on the uniform variable
    const GLint location = glGetUniformLocation(program, uniform_name.c_str());
    glUniform1i(location, texture_unit_number);

    deactivate();
  }

  void setImageTexture(const Texture& texture, GLuint image_unit_number,
                       GLenum access) const {
    activate();

    // bind texture to the specified image unit
    glActiveTexture(GL_TEXTURE0 + image_unit_number);
    glBindImageTexture(image_unit_number, texture.getTextureName(), 0, GL_FALSE,
                       0, access, texture.getTextureFormat());

    deactivate();
  }
};

}  // namespace gcss

#endif