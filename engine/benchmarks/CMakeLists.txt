project (chess_ann_benchmarks)

# It's not great, I know.
file(GLOB chess_ann_benchmarksfiles "*.h" "*.cpp" "**/*.h" "**/*.cpp" "../src/*.h" "../src/*.cpp" "../src/**/*.h" "../src/**/*.cpp")

# exclude src/main to avoid multiple main files.
list(REMOVE_ITEM chess_ann_benchmarksfiles "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

add_executable(chess_ann_benchmarks ${chess_ann_benchmarksfiles})

target_link_libraries(chess_ann_benchmarks benchmark stockfish -lpthread)
