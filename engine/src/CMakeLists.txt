# Settings
project (chess_ann_src)

# Files to be compiled.
file(GLOB chess_ann_srcfiles "*.cpp" "**/*.cpp")
add_executable(chess_ann_src ${chess_ann_srcfiles})

# Make this folder linkable for parent folder.
target_link_libraries(chess_ann_src fann -lpthread)