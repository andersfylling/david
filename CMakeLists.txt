# settings
set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.8)
project(chess_ann LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup paths
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Paths.cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# In case neither release nor debug is set as build type, default to release.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC -Wno-unused-variable")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Third party libraries in lib
add_subdirectory(external)

# Our source code
add_subdirectory(engine)

#MESSAGE( WARNING "BUILD TYPE" ${CMAKE_BUILD_TYPE})
#MESSAGE( WARNING "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )
#message(WARNING "CMAKE_COMMAND: " ${CMAKE_COMMAND})