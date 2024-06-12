cmake_minimum_required(VERSION 3.10)
project(basket)

include_directories("${PROJECT_SOURCE_DIR}/nms/glad/include/")

add_executable(engine main.cpp multiplayer.cpp events.cpp "${PROJECT_SOURCE_DIR}/gfx/shader.cpp" "${PROJECT_SOURCE_DIR}/nms/glad/src/gl.c")

if (WIN32)
    set(CMAKE_CXX_FLAGS "--static")
endif()

target_link_libraries(engine glfw3)

if(WIN32)
    target_link_libraries(engine gdi32 opengl32 dl ws2_32 wsock32)
endif()