add_executable(tone-mapping "src/main.cpp")
target_compile_features(tone-mapping PRIVATE cxx_std_20)
set_target_properties(tone-mapping PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(tone-mapping PRIVATE src)
target_link_libraries(tone-mapping PRIVATE gcss)
target_link_libraries(tone-mapping PRIVATE glfw)
target_link_libraries(tone-mapping PRIVATE imgui)
target_link_libraries(tone-mapping PRIVATE imgui_glfw_opengl3)

# set cmake source dir macro
target_compile_definitions(tone-mapping PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")