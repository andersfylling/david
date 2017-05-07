# CMake generated Testfile for 
# Source directory: /home/anders/Projects/chess_ann/external/Catch
# Build directory: /home/anders/Projects/chess_ann/cmake-build-debug/external/Catch
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(RunTests "/home/anders/Projects/chess_ann/cmake-build-debug/external/Catch/SelfTest")
add_test(ListTests "/home/anders/Projects/chess_ann/cmake-build-debug/external/Catch/SelfTest" "--list-tests")
set_tests_properties(ListTests PROPERTIES  PASS_REGULAR_EXPRESSION "[0-9]+ test cases")
add_test(ListTags "/home/anders/Projects/chess_ann/cmake-build-debug/external/Catch/SelfTest" "--list-tags")
set_tests_properties(ListTags PROPERTIES  PASS_REGULAR_EXPRESSION "[0-9]+ tags")
