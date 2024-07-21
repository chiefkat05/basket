cmake_minimum_required(VERSION 3.10)
project(basket)

include_directories("${CMAKE_SOURCE_DIR}/nms/glad/include/")

add_executable(engine main.cpp "${CMAKE_SOURCE_DIR}/logic/game.cpp" multiplayer.cpp "${CMAKE_SOURCE_DIR}/gfx/graphics.cpp" "${CMAKE_SOURCE_DIR}/logic/events.cpp" "${CMAKE_SOURCE_DIR}/logic/world.cpp" "${CMAKE_SOURCE_DIR}/logic/collision.cpp" "${CMAKE_SOURCE_DIR}/gfx/shader.cpp" "${CMAKE_SOURCE_DIR}/nms/glad/src/gl.c")

set(CMAKE_CXX_FLAGS "-O1 -Wall -I${CMAKE_SOURCE_DIR}/gfx/backpack/")

if (WIN32)
    set(CMAKE_CXX_FLAGS "--static -O1 -Wall")
endif()

if (UNIX)
    target_link_libraries(engine glfw3 tinyobjloader)
endif()

if(WIN32)
    target_link_libraries(engine glfw3 tinyobjloader gdi32 opengl32 dl ws2_32 wsock32)
endif()                  
