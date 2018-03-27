cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(FindPkgConfig)
if (PkgConfig_FOUND)
    pkg_search_module(GSTREAMER REQUIRED gstreamer-1.0)
    if (GSTREAMER_FOUND)
        include_directories(${GSTREAMER_INCLUDE_DIRS})
        link_directories(${GSTREAMER_LIBRARY_DIRS})
        link_libraries(${GSTREAMER_LIBRARIES})
    endif (GSTREAMER_FOUND)
endif (PkgConfig_FOUND)

add_executable(gstreamer gstreamer.cpp)
