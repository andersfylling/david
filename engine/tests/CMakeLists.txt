project (chess_ann_tests)


# It's not great, I know.
file(GLOB chess_ann_testsfiles "*.cpp" "**/*.cpp" "../src/*.cpp" "../src/**/*.cpp")

# exclude src/main to avoid multiple main files.
list(REMOVE_ITEM chess_ann_testsfiles "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

add_executable(chess_ann_tests ${chess_ann_testsfiles})

# Define DAVID_TESTS
target_compile_definitions(chess_ann_tests PUBLIC DAVID_TEST) # should be in a if sentence for specific builds
# add_definitions(-DDAVID_UNIT_TESTING)

target_link_libraries(chess_ann_tests fann uci -lpthread)

