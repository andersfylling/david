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
add_subdirectory(tests)     # Our tests

# cpp files in root project folder
set(SOURCE_FILES
    src/main.cpp
        src/enviornment.cpp
    )
add_executable(chess_ann ${SOURCE_FILES})

# link all folders (root + subfolders)
target_link_libraries(chess_ann)