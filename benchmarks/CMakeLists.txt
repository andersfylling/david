project (chess_ann_tests)
include(CheckCXXCompilerFlag)


ADD_EXECUTABLE(chess_ann_benchmarks main.cpp)
target_link_libraries(chess_ann_benchmarks -lpthread)
