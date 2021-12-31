#include <cstdio>
#include <iostream>
#include <memory>

#include "glad/gl.h"
//
#include "GLFW/glfw3.h"
//
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//
#include "renderer.h"

std::unique_ptr<Renderer> RENDERER;

static void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  RENDERER->setResolution(glm::uvec2(width, height));
}

void handleInput(GLFWwindow* window, const ImGuiIO& io) {
  // close window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

int main() {
  // init glfw
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    return -1;
  }

  // init window and OpenGL context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // required for Mac
  GLFWwindow* window =
      glfwCreateWindow(512, 512, "life-game", nullptr, nullptr);
  if (!window) {
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // init glad
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    std::cerr << "failed to initialize OpenGL context" << std::endl;
    return -1;
  }

  // init imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  // set imgui style
  ImGui::StyleColorsDark();

  // init imgui backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460 core");

  // init renderer
  RENDERER = std::make_unique<Renderer>();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("UI");
    {}
    ImGui::End();

    handleInput(window, io);

    // render
    RENDERER->render();

    // render imgui
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // cleanup
  RENDERER->destroy();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}