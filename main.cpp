#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "biginteger.h"

#include <memory>

#include <cstdio>
#include <cmath>
#include <cctype>

template<uint8_t radix>
struct bigint_t {
    typedef uint8_t digit_t;

    std::vector<digit_t> digits;

    bigint_t() {

    }

    bigint_t(uint32_t val, size_t rank=0) {
        while(rank--) {
            digits.push_back(0);
        }
        for(;val;val /= radix) {
            digits.push_back(val % radix);
        }
    }

    bigint_t(size_t count, digit_t value):digits(count,value) {

    }

    bigint_t(const std::string &s) {
        digits.reserve(s.size());

        digit_t val = 0;
        for(auto i=s.rbegin();i!=s.rend();++i) {
            if(isdigit(*i)) {
                val = *i - '0';
            } else if(isupper(*i)) {
                val = *i - 'A' + 10;
            } else if(islower(*i)) {
                val = *i - 'a' + 36;
            } else {
                break;
            }

            if(val >= radix) break;
            digits.push_back(val);
        }
    }

    bigint_t& operator+=(const bigint_t &other) {
        if(other.digits.size() > digits.size()) {
            digits.resize(other.digits.size());
        }

        auto a = digits.begin();
        auto b = other.digits.begin();

        digit_t extra = 0;
        for(;a != digits.end() && b != other.digits.end();++a,++b) {
            uint16_t val = *a + *b + extra;
            *a = val % radix;
            extra = val / radix;
        }

        for(;extra && a != digits.end();++a) {
            uint16_t val = *a + extra;
            *a = val % radix;
            extra = val / radix;
        }

        if(extra && a == digits.end()) {
            digits.push_back(extra);
        }

        return *this;
    }

    bigint_t& operator-=(const bigint_t &other) {
        if(other.digits.size() > digits.size()) {
            digits.resize(other.digits.size());
        }

        auto a = digits.begin();
        auto b = other.digits.begin();

        digit_t extra = 0;
        for(;a != digits.end() && b != other.digits.end();++a,++b) {
            uint16_t val = radix + *a - *b - extra;
            *a = val % radix;
            extra = 1 - val / radix;
        }

        for(;extra && a != digits.end();++a) {
            uint16_t val = radix + *a - extra;
            *a = val % radix;
            extra = 1 - val / radix;
        }

        if(extra && a == digits.end()) {
            digits.push_back(radix-1);
        }

        return *this;
    }

    bigint_t operator+(const bigint_t &other) const {
        bigint_t sum(std::max(digits.size(),other.digits.size()),0);

        sum.digits.assign(digits.begin(),digits.end());

        auto a = sum.digits.begin();
        auto b = other.digits.begin();

        digit_t extra = 0;
        for(;a != sum.digits.end() && b != other.digits.end();++a,++b) {
            uint16_t val = *a + *b + extra;
            *a = val % radix;
            extra = val / radix;
        }

        for(;extra && a != sum.digits.end();++a) {
            uint16_t val = *a + extra;
            *a = val % radix;
            extra = val / radix;
        }

        if(extra && a == sum.digits.end()) {
            sum.digits.push_back(extra);
        }

        return sum;
    }

    bigint_t operator*(const bigint_t &other) const {
        bigint_t total(0);

        for(auto a=digits.begin();a!=digits.end();++a) {
            for(auto b=other.digits.begin();b!=other.digits.end();++b) {
                size_t rank = (a - digits.begin()) + (b - other.digits.begin());
                uint16_t p = (uint16_t)*a * *b;
                bigint_t product(p,rank);
                total += product;
            }
        }

        return total;
    }

    uint32_t operator%(uint32_t n) const {
        uint32_t total = 0;
        uint32_t base = 1;
        for(auto i: digits) {
            total += (i % n) * base;
            total %= n;
            base = (base*radix) % n;
        }
        return total;
    }

    bigint_t operator/(uint32_t n) const {
        bigint_t total(0);



        return total;
    }

    std::string toString() const {
        auto buffer = std::make_unique<char[]>(digits.size());
        char* out = buffer.get() + digits.size();
        for(auto n: digits) {
            *--out = "0123456789ABCDEFGHIJKLMNOPQRSTYVWXYZabcdefghijklmnopqrstyvwxyz"[n];
        }

        return std::string(out,digits.size());
    }

    template<uint8_t new_radix>
    explicit operator bigint_t<new_radix>() const {
        return bigint_t<new_radix>(0);
    }
};

TEST_CASE("bigint_t-1","") {
    const std::string &a_str = "123456789ABCDEF";
    bigint_t<16> a(a_str);
    REQUIRE(a.toString() == a_str);
}

TEST_CASE("bigint_t-2","") {
    bigint_t<16> a("123456789ABCDEF");
    bigint_t<16> b("FEDCBA987654321");

    REQUIRE((a%16) == 0xF);
    REQUIRE((b%16) == 0x1);

    REQUIRE((a%256) == 0xEF);
    REQUIRE((b%256) == 0x21);

    REQUIRE((a+b).toString() == "1111111111111110");

    REQUIRE((a*b).toString() == "121FA00AD77D7422236D88FE5618CF");
}


TEST_CASE("BigInteger", "sum") {
    BigInteger a("FFFFFFFF",16);
    BigInteger b("FFFFFFFF",16);

    REQUIRE((a+b).toString(16) == "1FFFFFFFE");
}
