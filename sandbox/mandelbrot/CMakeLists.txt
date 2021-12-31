add_executable(mandelbrot "src/main.cpp")
target_compile_features(mandelbrot PRIVATE cxx_std_20)
set_target_properties(mandelbrot PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(mandelbrot PRIVATE src)
target_link_libraries(mandelbrot PRIVATE gcss)
target_link_libraries(mandelbrot PRIVATE glfw)
target_link_libraries(mandelbrot PRIVATE imgui)
target_link_libraries(mandelbrot PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(mandelbrot PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")