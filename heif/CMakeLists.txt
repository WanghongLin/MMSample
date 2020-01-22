
cmake_minimum_required(VERSION 3.1)

project(heif)

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:$ENV{HOME}/Developments/github/libheif/out/lib/pkgconfig")

include(FindPkgConfig)

if (PkgConfig_FOUND)
    pkg_check_modules(LIBHEIF libheif)
    if (LIBHEIF_FOUND)
        include_directories(${LIBHEIF_INCLUDE_DIRS})
        link_directories(${LIBHEIF_LIBRARY_DIRS})
        link_libraries(${LIBHEIF_LIBRARIES})
    endif (LIBHEIF_FOUND)
endif (PkgConfig_FOUND)

add_executable(rgba2heif rgba2heif.cpp)