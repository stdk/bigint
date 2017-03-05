#include "biginteger.h"

#include <algorithm>
#include <cctype>

using namespace std;

BigInteger::BigInteger(base_t n):digits{n}
{

}

BigInteger::BigInteger(const BigInteger &other):digits(other.digits) {

}

BigInteger::BigInteger(const std::string &s, size_t base) {
    base_t b = 1;
    base_t val = 0;
    for(auto i=s.rbegin();i!=s.rend();++i) {
        if(isdigit(*i)) {
            val = *i - '0';
        } else if(isupper(*i)) {
            val = *i - 'A' + 10;
        } else if(islower(*i)) {
            val = *i - 'a' + 36;
        }
        b *= base;
    }
}

string BigInteger::toString(size_t base) const {
    size_t max_len = digits.size()*8;
    char *buffer = new char[max_len];
    char *out = buffer + max_len;
    for(auto n: digits) {
        for(; out != buffer && n; n/=base) {
            *--out = "0123456789ABCDEFGHIJKLMNOPQRSTYVWXYZabcdefghijklmnopqrstyvwxyz"[n % base];
        }
    }

    return string(out,buffer + max_len - out);
}

int BigInteger::compare(const BigInteger &other) const {
    if(digits.size() == other.digits.size()) {
        auto a = digits.rbegin();
        auto b = other.digits.rbegin();
        while(a != digits.rend() && *a++ == *b++);

        if(a == digits.rend()) {
            return 0;
        }

        return *a - *b;
    } else {
        return digits.size() - other.digits.size();
    }
}

bool BigInteger::operator<(const BigInteger &other) const {

}

BigInteger BigInteger::operator+(const BigInteger &other) const {
    BigInteger total(*this);

    if(total.digits.size() < other.digits.size()) {
        total.digits.resize(other.digits.size());
    }

    auto a = total.digits.begin();
    auto b = other.digits.begin();

    bool extra = false;
    for(;a != total.digits.end() && b != other.digits.end();++a,++b) {
        if(extra) {
            *a += 1;
            extra = !*a;
        }
        if(!*a) {
            *a = *b;
        } else {
            base_t val = *a + *b;
            if(val < *a) {
                extra = true;
            }
            *a = val;
        }
    }

    for(;a != total.digits.end();++a) {
        if(extra) {
            *a += 1;
            extra = !*a;
        }
    }

    if(extra) {
        total.digits.push_back(1);
    }

    return total;
}
