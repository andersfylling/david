# settings
cmake_minimum_required(VERSION 3.6)
project(chess_ann LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

# Setup paths
include(Paths.cmake)

# Third party libraries in lib
add_subdirectory(external)

# Our source code
add_subdirectory(engine)