
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(src)           # source files
add_subdirectory(benchmarks)    # Our benchmarks
add_subdirectory(tests)         # Our tests