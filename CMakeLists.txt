cmake_minimum_required(VERSION 3.5)
project(MMSample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

#move down to ffdemo
#set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/Applications/ffmpeg/lib/pkgconfig")
#
#include(FindPkgConfig)
#if (PkgConfig_FOUND)
#    pkg_check_modules(LIBFFMPEG libavformat libavcodec libavutil libswscale libswresample)
#    if (LIBFFMPEG_FOUND)
#        include_directories(BEFORE SYSTEM ${LIBFFMPEG_INCLUDE_DIRS})
#        link_directories(${LIBFFMPEG_LIBRARY_DIRS})
#        link_libraries("${LIBFFMPEG_LIBRARIES}")
#    endif (LIBFFMPEG_FOUND)
#
#    pkg_check_modules(SDL sdl2 SDL2_image)
#    if (SDL_FOUND)
#        include_directories(${SDL_INCLUDE_DIRS})
#        add_compile_options(${SDL_CFLAGS})
#        link_directories(${SDL_LIBRARY_DIRS})
#        link_libraries(${SDL_LIBRARIES})
#    endif (SDL_FOUND)
#endif (PkgConfig_FOUND)

add_subdirectory(png)
add_subdirectory(sdl)
add_subdirectory(gles)
add_subdirectory(gtk)
add_subdirectory(gstreamer)
add_subdirectory(misc)
add_subdirectory(apue)
add_subdirectory(cppPrimerPlus)
add_subdirectory(leetcode)
add_subdirectory(igesture)

add_subdirectory(heif)
add_subdirectory(ocv)
add_subdirectory(webp)
add_subdirectory(ffdemo)

add_subdirectory($ENV{HOME}/Developments/github/ffmpeg ffmpeg)