# settings
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

# root project name
project(chess_ann)

# sub folders
#add_subdirectory(chessBoard)

# cpp files in root project folder
set(SOURCE_FILES src/main.cpp)
add_executable(chess_ann ${SOURCE_FILES})

# link all folders (root + subfolders)
target_link_libraries(chess_ann)