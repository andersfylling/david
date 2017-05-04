# settings
cmake_minimum_required(VERSION 3.6)
project(chess_ann VERISON 0.3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

if(COMPILER_SUPPORTS_CXX14)
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++14 support.")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. C++14 or higher is required for this project.")
    return()
endif()

# Setup paths
include(Paths.cmake)

# Third party libraries in lib
add_subdirectory(external)

# Our source code
add_subdirectory(engine)