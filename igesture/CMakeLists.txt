

cmake_minimum_required(VERSION 3.5)
project(igesture)

add_executable(igesture igesture.c)

include(FindPkgConfig)
pkg_check_modules(INPUT libinput)
if (INPUT_FOUND)
    target_link_libraries(igesture ${INPUT_LIBRARIES})
endif (INPUT_FOUND)

