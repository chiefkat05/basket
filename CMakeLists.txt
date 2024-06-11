cmake_minimum_required(VERSION 3.10)
project(basket)
configure_file(version.h.in version.h)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set(CMAKE_CXX_FLAGS "--static -L/usr/x86_64-w64-mingw32/lib/ -L/usr/lib/")

# add_subdirectory( "${PROJECT_SOURCE_DIR}/../../CxxDeps/glfw-3.4/"  CMAKE_BINARY_DIR)

include_directories("${PROJECT_SOURCE_DIR}/nms/glad/include/")

add_executable(engine main.cpp multiplayer.cpp events.cpp "${PROJECT_SOURCE_DIR}/gfx/shader.cpp" "${PROJECT_SOURCE_DIR}/nms/glad/src/gl.c")
# target_link_libraries(engine glfw)

set(CMAKE_CXX_FLAGS "--static")
target_link_libraries(engine glfw3 gdi32 opengl32 dl)
