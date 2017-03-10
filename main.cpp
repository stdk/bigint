#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "bigint.h"

TEST_CASE("bigint_t-1","") {
    const std::string &a_str = "123456789ABCDEF";
    bigint_t<16> a(a_str);
    REQUIRE(a.toString() == a_str);

    REQUIRE((bigint_t<16>("123456789000000") + bigint_t<16>(0xABCDEF)).toString() == a_str);

    REQUIRE(static_cast<bigint_t<10>>(bigint_t<16>("AAABBBCCCDDDEEEFFF")).toString() == "3148321202116318130175");
}

TEST_CASE("bigint_t-2","") {
    bigint_t<16> a("123456789ABCDEF0123456789ABCDEF");
    bigint_t<16> b("FEDCBA9876543210FEDCBA987654321");

    REQUIRE((b-a).toString() == "ECA8641FDB975320ECA8641FDB97532");

    REQUIRE((a%16) == 0xF);
    REQUIRE((b%16) == 0x1);

    REQUIRE((a%256) == 0xEF);
    REQUIRE((b%256) == 0x21);

    REQUIRE((a%(16*16*16)) == 0xDEF);
    REQUIRE((b%(16*16*16)) == 0x321);

    REQUIRE((a+b).toString() == "11111111111111101111111111111110");

    REQUIRE((a*b).toString() == "121FA00AD77D742247ACC9140513B74458FAB20783AF1222236D88FE5618CF");

    REQUIRE((b/0x12345679).toString() == "DFFFFFFB2200001C02DFFF64");

    REQUIRE((b/bigint_t<16>(0x12345679)).toString() == "DFFFFFFB2200001C02DFFF64");
}
