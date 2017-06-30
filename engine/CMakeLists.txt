
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(${CHESS_ANN_BENCHMARK_INCLUDE_DIR})
include_directories(${CHESS_ANN_CATCH_INCLUDE_DIR})
include_directories(${CHESS_ANN_FANN_INCLUDE_DIR})
include_directories(${CHESS_ANN_STOCKFISH_INCLUDE_DIR})
include_directories(${CHESS_ANN_UCI_INCLUDE_DIR})

add_subdirectory(src)           # source files
add_subdirectory(src_debug)     # linked source files, but with the DAVID_DEBUG macro activated
add_subdirectory(benchmarks)    # Our benchmarks
add_subdirectory(tests)         # Our tests
add_subdirectory(ANNTraining)   # Neural network training