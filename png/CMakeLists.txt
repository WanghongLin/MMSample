project(png)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    set(CMAKE_PREFIX_PATH "$ENV{HOME}/Downloads/libpng-1.6.25/out/lib/pkgconfig")
    set(ENV{PKG_CONFIG_PATH} "$ENV{HOME}/Downloads/libpng-1.6.25/out/lib/pkgconfig")
    pkg_check_modules(PNG libpng=1.6.25)
    if (PNG_FOUND)
        message($ENV{PKG_CONFIG_PATH})
        include_directories("${PNG_INCLUDE_DIRS}")
        link_directories("${PNG_LIBRARY_DIRS}")
        link_libraries("${PNG_LIBRARIES}")
    endif (PNG_FOUND)
endif (PKG_CONFIG_FOUND)

add_executable(png_encoder png_encoder.cpp)