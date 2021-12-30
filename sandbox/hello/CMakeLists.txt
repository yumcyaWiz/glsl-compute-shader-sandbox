add_executable(hello "src/main.cpp")
target_compile_features(hello PRIVATE cxx_std_20)
set_target_properties(hello PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(hello PRIVATE src)
target_link_libraries(hello PRIVATE css)
target_link_libraries(hello PRIVATE glfw)
target_link_libraries(hello PRIVATE imgui)
target_link_libraries(hello PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(hello PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")