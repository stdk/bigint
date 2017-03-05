#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <cstdint>
#include <vector>
#include <string>

class BigInteger
{
public:
    typedef uint32_t base_t;

    BigInteger(base_t);
    BigInteger(const BigInteger&);
    BigInteger(const std::string&, size_t base=10);

    std::string toString(size_t base=10) const;

    BigInteger operator+(const BigInteger&) const;

    int compare(const BigInteger&) const;
    bool operator<(const BigInteger&) const;
private:
    std::vector<base_t> digits;
};

#endif // BIGINTEGER_H
