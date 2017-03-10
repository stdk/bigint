#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cctype>

template<uint8_t radix>
class bigint_t {
public:
    typedef uint8_t digit_t;

private:
    std::vector<digit_t> digits;

    uint32_t guess_divisor(const bigint_t &other) const {
        if(rank() < 2) {
            return *digits.rbegin()/(*other.digits.rbegin());
        }

        auto u = digits.rbegin();
        auto v = other.digits.rbegin();

        uint32_t a = (uint32_t)*u*radix + *(u+1);
        uint32_t b = (uint32_t)*v*radix + *(v+1);
        return std::min((uint32_t)radix-1,a/b);
    }

    void erase_leading_zeros() {
        auto i = digits.rbegin();
        for(;i != digits.rend() && !*i;++i);
        digits.resize(digits.rend() - i);
    }

public:
    inline size_t rank() const {
        return digits.size();
    }

    bigint_t() {

    }

    bigint_t(uint64_t val, size_t rank=0) {
        if(!val) {
            digits.reserve(rank);
        } else {
            digits.reserve(ceil(log(val+1)/log(radix))+rank);
            while(rank--) {
                digits.push_back(0);
            }
            for(;val;val /= radix) {
                digits.push_back(val % radix);
            }
        }
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

    int compare(const bigint_t &other) const {
        if(rank() == other.rank()) {
            auto a = digits.rbegin();
            auto b = other.digits.rbegin();
            for(;a != digits.rend() && *a == *b;++a,++b);

            if(a == digits.rend()) {
                return 0;
            }

            return *a - *b;
        } else {
            return rank() - other.rank();
        }
    }

    inline bool operator<(const bigint_t &other) const {
        return compare(other) < 0;
    }

    inline bool operator>(const bigint_t &other) const {
        return compare(other) > 0;
    }

    inline bool operator<=(const bigint_t &other) const {
        return compare(other) <= 0;
    }

    inline bool operator>=(const bigint_t &other) const {
        return compare(other) >= 0;
    }

    inline bool operator==(const bigint_t &other) const {
        return compare(other) == 0;
    }

    inline bool operator!=(const bigint_t &other) const {
        return compare(other) != 0;
    }

    bigint_t& operator+=(const bigint_t &other) {
        if(rank() < other.rank()) {
            digits.reserve(other.rank()+1);
            digits.resize(other.rank());
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
        if(rank() < other.rank()) {
            digits.reserve(other.rank()+1);
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

        for(;b != other.digits.end();++b) {
            uint16_t val = radix - *b - extra;
            digits.push_back(val % radix);
            extra = 1 - val / radix;
        }

        if(extra && a == digits.end()) {
            digits.push_back(radix-1);
        }

        erase_leading_zeros();

        return *this;
    }

    bigint_t operator+(const bigint_t &other) const {
        bigint_t sum(0,std::max(rank(),other.rank())+1);
        sum.digits.assign(digits.begin(),digits.end());
        sum += other;
        return sum;
    }

    bigint_t operator-(const bigint_t &other) const {
        bigint_t sum(0,std::max(rank(),other.rank())+1);
        sum.digits.assign(digits.begin(),digits.end());
        sum -= other;
        return sum;
    }

    bigint_t operator<<(size_t shift) const {
        size_t new_size = rank()+shift;
        bigint_t result(0,new_size);
        result.digits.resize(new_size);
        std::copy(digits.begin(),digits.end(),result.digits.begin()+shift);
        return result;
    }

    bigint_t operator>>(size_t shift) const {
        if(shift >= rank()) {
            return bigint_t(0);
        }

        size_t new_size = rank() - shift;
        bigint_t result(0,new_size);
        result.digits.resize(new_size);
        std::copy(digits.begin()+shift,digits.end(),result.digits.begin());
        return result;
    }

    bigint_t operator*(const bigint_t &other) const {
        bigint_t total(0,rank()+other.rank()+1);

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
        bigint_t total(0,rank());

        uint64_t current = 0;

        for(auto i=digits.rbegin();i!=digits.rend();++i) {
            current = current*radix + *i;
            if(current >= n) {
                size_t rank = digits.rend() - i - 1;
                total += bigint_t(current/n,rank);
                current %= n;
            }
        }

        return total;
    }

    bigint_t operator/(const bigint_t &other) const {
        bigint_t total(0,rank());
        bigint_t current(*this);

        for(int i=0;current.compare(other) >= 0;++i) {
            auto q = current.guess_divisor(other);
            bigint_t sub = other*bigint_t(q);
            int rank_diff = std::max(0,(int)current.rank() - (int)sub.rank());
            bigint_t head = current >> rank_diff;

//            fprintf(stderr,"1cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                    current.toString().c_str(),
//                    other.toString().c_str(),
//                    q,
//                    rank_diff,
//                    head.toString().c_str(),
//                    sub.toString().c_str());

            for(;q && sub > head;--q) {
                sub -= other;
            }

//            fprintf(stderr,"2cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                    current.toString().c_str(),
//                    other.toString().c_str(),
//                    q,
//                    rank_diff,
//                    head.toString().c_str(),
//                    sub.toString().c_str());

            if(!q && rank_diff) {
                q = radix - 1;
                sub = other*bigint_t(q);
                rank_diff = std::max(0,(int)current.rank() - (int)sub.rank());
//                bigint_t head = current >> rank_diff;

//                fprintf(stderr,"3cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                        current.toString().c_str(),
//                        other.toString().c_str(),
//                        q,
//                        rank_diff,
//                        head.toString().c_str(),
//                        sub.toString().c_str());
            }

            current -= sub << rank_diff;

            total += bigint_t(q,rank_diff);
        }

        int cmp = current.compare(other);
        if(cmp == 0) {
            return total + bigint_t(1);
        }
        return total;
    }

    std::string toString() const {
        auto buffer = std::make_unique<char[]>(rank());
        char* out = buffer.get() + rank();
        for(auto n: digits) {
            *--out = "0123456789ABCDEFGHIJKLMNOPQRSTYVWXYZabcdefghijklmnopqrstyvwxyz"[n];
        }

        return std::string(out,rank());
    }

    template<uint8_t new_radix>
    explicit operator bigint_t<new_radix>() const {
        return bigint_t<new_radix>(0);
    }
};

#endif // BIGINTEGER_H
