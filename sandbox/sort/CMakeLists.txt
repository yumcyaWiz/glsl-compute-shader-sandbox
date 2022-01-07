add_executable(sort "src/main.cpp")
target_compile_features(sort PRIVATE cxx_std_20)
set_target_properties(sort PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sort PRIVATE src)
target_link_libraries(sort PRIVATE gcss)
target_link_libraries(sort PRIVATE glfw)
target_link_libraries(sort PRIVATE imgui)
target_link_libraries(sort PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(sort PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")