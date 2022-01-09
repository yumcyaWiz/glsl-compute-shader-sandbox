#include <cstdio>
#include <iostream>

#include "glad/gl.h"
//
#include "GLFW/glfw3.h"
//
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
//
#include "renderer.h"

Renderer* RENDERER;

static void glfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window,
                                    int width, int height) {
  RENDERER->setResolution(glm::uvec2(width, height));
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
  GLFWwindow* window = glfwCreateWindow(512, 512, "hello", nullptr, nullptr);
  if (!window) {
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // enable vsync

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
  RENDERER = new Renderer();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("UI");
    {
      static float exposure = RENDERER->getExposure();
      if (ImGui::SliderFloat("Exposure", &exposure, 0, 10)) {
        RENDERER->setExposure(exposure);
      }

      static bool tone_mapping_on_rgb = RENDERER->getToneMappingOnRGB();
      if (ImGui::Checkbox("Tone mapping on RGB", &tone_mapping_on_rgb)) {
        RENDERER->setToneMappingOnRGB(tone_mapping_on_rgb);
      }

      static int tone_mapping_type =
          static_cast<int>(RENDERER->getToneMappingType());
      if (ImGui::Combo("Tone mapping function", &tone_mapping_type,
                       "Linear\0Reinhard\0ACES\0Uchimura\0\0")) {
        RENDERER->setToneMappingType(
            static_cast<ToneMappingType>(tone_mapping_type));
      }

      static float gamma = RENDERER->getGamma();
      if (ImGui::SliderFloat("Gamma", &gamma, 0, 3)) {
        RENDERER->setGamma(gamma);
      }
    }
    ImGui::End();

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
  delete RENDERER;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}