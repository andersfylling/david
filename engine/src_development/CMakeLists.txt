project (chess_ann_src_development)


# It's not great, I know.
file(GLOB chess_ann_src_developmentfiles "*.cpp" "**/*.cpp" "../src/*.cpp" "../src/**/*.cpp")

# exclude src/main to avoid multiple main files.
#list(REMOVE_ITEM chess_ann_testsfiles "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

add_executable(chess_ann_src_development ${chess_ann_src_developmentfiles})

# Define DAVID_UNIT_TESTING
target_compile_definitions(chess_ann_src_development PUBLIC DAVID_DEBUG) # should be in a if sentence for specific builds
target_compile_definitions(chess_ann_src_development PUBLIC DAVID_DEVELOPMENT) # should be in a if sentence for specific builds
# add_definitions(-DDAVID_UNIT_TESTING)

target_link_libraries(chess_ann_src_development fann uci -lpthread)

