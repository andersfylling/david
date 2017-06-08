project (chess_ann_anntraining)

# It's not great, I know.
file(GLOB chess_ann_anntrainingfiles "*.cpp" "**/*.cpp" "../src/*.cpp" "../src/**/*.cpp")

# exclude src/main to avoid multiple main files.
list(REMOVE_ITEM chess_ann_anntrainingfiles "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

add_executable(chess_ann_anntraining  ${chess_ann_anntrainingfiles})


target_link_libraries(chess_ann_anntraining fann uci -lpthread)

