add_executable(particles "src/main.cpp")
target_compile_features(particles PRIVATE cxx_std_20)
set_target_properties(particles PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(particles PRIVATE src)
target_link_libraries(particles PRIVATE gcss)
target_link_libraries(particles PRIVATE glfw)
target_link_libraries(particles PRIVATE imgui)
target_link_libraries(particles PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(particles PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")