cmake_minimum_required(VERSION 3.20)
project(atmos LANGUAGES C CXX)

option(BUILD_TESTS "build tests" OFF)

# OpenGL
find_package(OpenGL REQUIRED)

# externals
add_subdirectory("externals")

# compute-shader-sandbox
add_library(css INTERFACE)
target_include_directories(css INTERFACE "include")
target_compile_features(css INTERFACE cxx_std_20)
set_target_properties(css PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(css INTERFACE OpenGL::GL)
target_link_libraries(css INTERFACE glad)
target_link_libraries(css INTERFACE glfw)
target_link_libraries(css INTERFACE imgui)
target_link_libraries(css INTERFACE imgui_glfw_opengl3)
target_link_libraries(css INTERFACE glm)
target_link_libraries(css INTERFACE spdlog::spdlog)

# sandbox
add_subdirectory("sandbox")

# tests
if(BUILD_TESTS)
  add_subdirectory("tests")
endif()