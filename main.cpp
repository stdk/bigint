#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "biginteger.h"

TEST_CASE("BigInteger", "sum") {
    BigInteger a("FFFFFFFF",16);
    BigInteger b("FFFFFFFF",16);

    REQUIRE((a+b).toString(16) == "1FFFFFFFE");
}
