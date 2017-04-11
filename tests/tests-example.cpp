#include "catch.hpp"
#include "../src/include/TestExample.h"

TEST_CASE( "Double size are being computed", "[TestExample.doubleMyInt]" ) {
  TestExample testExample;

  REQUIRE( testExample.doubleMyInt(1) == 2 );
  REQUIRE( testExample.doubleMyInt(2) == 4 );
  REQUIRE( testExample.doubleMyInt(3) == 6 );
  REQUIRE( testExample.doubleMyInt(10) == 20 );
}
