# settings
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

if(COMPILER_SUPPORTS_CXX14)
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++14 support.")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. C++14 or higher is required for this project.")
    return()
endif()

# Store this as the root directory
set(ROOT_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Third party libraries in lib
add_subdirectory(lib/Catch)     # Catch, for unit testing.
add_subdirectory(lib/fann)      # Fann, for faster neural network development.
add_subdirectory(lib/benchmark) # Benchmark, google benchmarks for easier decision making and overview of code performance.

add_subdirectory(src)           # source files
add_subdirectory(benchmarks)    # Our benchmarks
add_subdirectory(tests)         # Our tests