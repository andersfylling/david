project (chess_ann_tests)
include(CheckCXXCompilerFlag)

# It's not great, I know.
file(GLOB chess_ann_testsfiles "*.h" "*.cpp" "**/*.h" "**/*.cpp" "../engine/src/*.h" "../engine/src/*.cpp" "../engine/src/**/*.h" "../engine/src/**/*.cpp")

# exclude src/main to avoid multiple main files.
list(REMOVE_ITEM chess_ann_testsfiles "../engine/src/main.cpp")

add_executable(chess_ann_tests  ${chess_ann_testsfiles})

target_link_libraries(chess_ann_tests -lpthread)
