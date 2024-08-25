#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "design.h"

TEST_CASE("Factorials are computed", "[factorial]")
{
    REQUIRE(factorial(0) == 0);
    REQUIRE(factorial(1) == 1);
}
