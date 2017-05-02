project (chess_ann_tests)
include(CheckCXXCompilerFlag)


ADD_EXECUTABLE(chess_ann_tests
        ../src/TestExample.cpp
        ../src/uci/Listener.cpp
        ../src/uci/Parser.cpp
        ../src/uci/UCIEvent.h

    tests-main.cpp
        ../src/enviornment.cpp
    moveGenTests.cpp
        tests-UCIListener.cpp
        tests-UCIHandler.cpp

        ../src/uci/Definitions.h ../src/uci/Response.h)
target_link_libraries(chess_ann_tests -lpthread)
