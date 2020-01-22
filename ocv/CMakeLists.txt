
cmake_minimum_required(VERSION 3.1)
project(ocv)

set(ENV{PKG_CONFIG_PATH} $ENV{HOME}/Developments/github/ocv_osx_install/lib/pkgconfig)

include(FindPkgConfig)
if (PkgConfig_FOUND)

    pkg_check_modules(OCV opencv4)
    if (OCV_FOUND)
        include_directories(${OCV_INCLUDE_DIRS})
        link_directories(${OCV_LIBRARY_DIRS})
        link_libraries(${OCV_LIBRARIES})
    endif (OCV_FOUND)

endif (PkgConfig_FOUND)

add_executable(display-image display-image.cpp)
add_executable(load-modify-save-image load-modify-save-image.cpp)
add_executable(mat-basic mat-basic.cpp)
add_executable(scan-image scan-image.cpp)
add_executable(mat-mask-operation mat-mask-operation.cpp)
add_executable(mat-operation mat-operation.cpp)
add_executable(mat_basic mat_basic.cpp)
add_executable(blending-two-image blending-two-image.cpp)
add_executable(basic-linear-transform basic-linear-transform.cpp)
add_executable(changing-constrast-brightness-image changing-constrast-brightness-image.cpp)

add_executable(basic-drawing basic-drawing.cpp)
add_executable(thresholding_simple thresholding_simple.cpp)
add_executable(random-drawing random-drawing.cpp)
add_executable(smoothing smoothing.cpp)
add_executable(morphology morphology.cpp)
add_executable(morphology-ex morphology-ex.cpp)