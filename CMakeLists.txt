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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/fann/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/Catch/include)

include_directories (${CMAKE_CURRENT_BINARY_DIR}/include)

ADD_SUBDIRECTORY(src)


ADD_SUBDIRECTORY(tests)

