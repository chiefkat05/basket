cmake_minimum_required(VERSION 3.10)
project(basket)
configure_file(version.h.in version.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory( "${PROJECT_SOURCE_DIR}/../../CxxDeps/glfw-3.4/"  CMAKE_BINARY_DIR)

include_directories("${PROJECT_SOURCE_DIR}/nms/glad/include/")

add_executable(engine main.cpp multiplayer.cpp events.cpp "${PROJECT_SOURCE_DIR}/gfx/shader.cpp" "${PROJECT_SOURCE_DIR}/nms/glad/src/gl.c")
target_link_libraries(engine glfw)
