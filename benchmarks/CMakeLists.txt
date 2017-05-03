project (chess_ann_benchmarks)
include(CheckCXXCompilerFlag)

# It's not great, I know.
file(GLOB chess_ann_benchmarksfiles "*.h" "*.cpp" "**/*.h" "**/*.cpp" "../src/*.h" "../src/*.cpp" "../src/**/*.h" "../src/**/*.cpp")

# exclude src/main to avoid multiple main files.
list(REMOVE_ITEM chess_ann_benchmarksfiles "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

add_executable(chess_ann_benchmarks ${chess_ann_benchmarksfiles} test.cpp include.h)

target_link_libraries(chess_ann_benchmarks benchmark -lpthread)
