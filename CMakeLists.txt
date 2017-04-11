# settings
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

# root project name
project(chess_ann)

# sub folders
#add_subdirectory(chessBoard)

# Third party libraries in lib
add_subdirectory(lib/Catch) # Catch, for unit testing.
add_subdirectory(lib/fann)  # Fann, for faster neural network development.
#target_link_libraries(chess_ann fann_static)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/fann/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/Catch/include)

# cpp files in root project folder
set(SOURCE_FILES
    src/main.cpp
    tests/tests-example.cpp
    src/TestExample.cpp
    src/include/TestExample.h)
add_executable(chess_ann ${SOURCE_FILES})


# link all folders (root + subfolders)
target_link_libraries(chess_ann fann)

ADD_SUBDIRECTORY(tests)