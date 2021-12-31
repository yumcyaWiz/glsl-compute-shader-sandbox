add_executable(n-body "src/main.cpp")
target_compile_features(n-body PRIVATE cxx_std_20)
set_target_properties(n-body PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(n-body PRIVATE src)
target_link_libraries(n-body PRIVATE gcss)
target_link_libraries(n-body PRIVATE glfw)
target_link_libraries(n-body PRIVATE imgui)
target_link_libraries(n-body PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(n-body PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")