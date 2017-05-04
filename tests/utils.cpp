#include "catch.h"
#include "../src/utils.h"

TEST_CASE("Verify that absolute path is working") {
  std::cout << utils::getAbsoluteProjectPath();
}