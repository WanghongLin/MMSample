project(ffdemo)

set(FFMPEG_INSTALL_ROOT "$ENV{HOME}/Developments/github/ffmpeg/out")
set(ENV{PKG_CONFIG_PATH} "${FFMPEG_INSTALL_ROOT}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/Developments/github/libwebp/out/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/Developments/github/x264/out/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/Developments/github/x265/out/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

message("$ENV{PKG_CONFIG_PATH}")

link_directories("$ENV{HOME}/Downloads/downloadsource/install/lib")
link_libraries("-lmp3lame")

include(FindPkgConfig)
if (PkgConfig_FOUND)
    pkg_check_modules(FFMPEG libavutil libavcodec libavformat libavfilter libswscale libswresample)
    if (FFMPEG_FOUND)
        include_directories("${FFMPEG_INCLUDE_DIRS}")
        link_directories("${FFMPEG_LIBRARY_DIRS}")
        link_libraries("${FFMPEG_LIBRARIES}")
    endif (FFMPEG_FOUND)

    pkg_check_modules(FFDEPS x264 x265 libwebp libwebpmux)
    if (FFDEPS_FOUND)
        include_directories("${FFDEPS_INCLUDE_DIRS}")
        link_directories("${FFDEPS_LIBRARY_DIRS}")
        link_libraries("${FFDEPS_LIBRARIES}")
    endif (FFDEPS_FOUND)

    pkg_check_modules(SDL sdl2 SDL2_image)
    if (SDL_FOUND)
        include_directories(${SDL_INCLUDE_DIRS})
        add_compile_options(${SDL_CFLAGS})
        link_directories(${SDL_LIBRARY_DIRS})
        link_libraries(${SDL_LIBRARIES})
    endif (SDL_FOUND)
endif (PkgConfig_FOUND)

# we need the following action for static libraries
string(REPLACE ";" " " ldflags "${FFMPEG_LDFLAGS}")
string(REPLACE ";" " " ldflags2 "${FFDEPS_LDFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ldflags} ${ldflags2}")

add_executable(dump_stream_info dump_stream_info.cpp)
add_executable(filter_naive filter_naive.cpp)
add_executable(extract_x265 extract_x265.cpp)
add_executable(extract_video extract_video.cpp)
add_executable(sine sine.cpp)
add_executable(transcode_naive transcode_naive.cpp)
add_executable(transcode_audio transcode_audio.cpp)
add_executable(transcode_video transcode_video.cpp)

link_libraries("pthread")
add_executable(aplayer aplayer.c)
add_executable(sws sws.c)
add_executable(extract_audio extract_audio.c)
target_link_libraries(aplayer "pthread")

