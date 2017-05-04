#include "catch.h"
#include "../engine/include/chess_ann/utils.h"

TEST_CASE("Verify that absolute path is working") {
  std::cout << utils::getAbsoluteProjectPath();
}