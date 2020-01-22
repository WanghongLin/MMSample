
cmake_minimum_required(VERSION 3.10)
project(webp)


set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/Developments/github/libwebp/out/lib/pkgconfig:$ENV{HOME}/Developments/github/ocv_osx_install/lib/pkgconfig")

include(FindPkgConfig)
if (PkgConfig_FOUND)

    pkg_check_modules(LIBWEBP libwebp libwebpdemux opencv4)
    if (LIBWEBP_FOUND)
        message("${LIBWEP_LIBRARIES}")
        include_directories(${LIBWEBP_INCLUDE_DIRS})
        link_directories(${LIBWEBP_LIBRARY_DIRS})
        link_libraries(${LIBWEBP_LIBRARIES})
    endif (LIBWEBP_FOUND)

endif (PkgConfig_FOUND)

add_executable(awebp awebp.cpp)