cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_libraries(EGL GLESv2 X11 png)
add_executable(gles gles.cpp png_texture.cpp common.h)
