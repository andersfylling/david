#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

TEST_CASE("Info") {
#ifdef DAVID_UNIT_TESTING
  std::cout << "Current MACRO: DAVID_UNIT_TESTING" << std::endl;
#endif
}