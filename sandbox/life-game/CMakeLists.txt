add_executable(life-game "src/main.cpp")
target_compile_features(life-game PRIVATE cxx_std_20)
set_target_properties(life-game PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(life-game PRIVATE src)
target_link_libraries(life-game PRIVATE gcss)
target_link_libraries(life-game PRIVATE glfw)
target_link_libraries(life-game PRIVATE imgui)
target_link_libraries(life-game PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(life-game PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")