project (chess_ann)
include(CheckCXXCompilerFlag)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)

CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++14 support.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++0x, C++11 support. chess_ann will still work with no problem, but the tests will not be able to compile.")
    return()
endif()


# cpp files in root project folder
set(SOURCE_FILES
    main.cpp
    TestExample.cpp
        ../include/evaluate.h evaluate.cpp ../include/variables.h)
add_executable(chess_ann ${SOURCE_FILES})


# link all folders (root + subfolders)

target_link_libraries(chess_ann fann)
