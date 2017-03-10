#ifndef BIGINT_H
#define BIGINT_H

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cctype>

struct bigint_t {
    typedef uint8_t digit_t;

    digit_t radix;
    std::vector<digit_t> digits;

    uint32_t guess_divisor(const bigint_t &other) const {
        if(radix != other.radix) {
            return guess_divisor(other.convertToRadix(radix));
        }

        if(rank() < 2 || other.rank() < 2) {
            return *digits.rbegin()/(*other.digits.rbegin());
        }

        auto u = digits.rbegin();
        auto v = other.digits.rbegin();

        uint32_t a = (uint32_t)*u*radix + *(u+1);
        uint32_t b = (uint32_t)*v*radix + *(v+1);

        uint32_t q = a/b;
        if(!q) {
            q = a/(*v);
        }
        return q;
    }

    void erase_leading_zeros() {
        auto i = digits.rbegin();
        for(;i != digits.rend() && !*i;++i);
        digits.resize(digits.rend() - i);
    }

    inline size_t rank() const {
        return digits.size();
    }

    bigint_t(uint64_t val, size_t rank, digit_t _radix):radix(_radix) {
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

    bigint_t(uint64_t val):bigint_t(val,0,255) {

    }

    bigint_t(const std::string &s,digit_t _radix):radix(255) {
        bigint_t tmp(0,s.size(),_radix);

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

            if(val >= _radix) break;
            tmp.digits.push_back(val);
        }

        //digits.reserve(ceil(s.size()/(log(radix)/log(_radix))));
        for(;tmp;tmp = tmp/radix) {
            digits.push_back(tmp%radix);
        }
    }

    int compare(const bigint_t &other) const {

        if(radix != other.radix) {
            return compare(other.convertToRadix(radix));
        }

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
        if(radix != other.radix) {
            return operator+=(other.convertToRadix(radix));
        }

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
        if(radix != other.radix) {
            return operator-=(other.convertToRadix(radix));
        }

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
        if(radix != other.radix) {
            return operator+(other.convertToRadix(radix));
        }

        bigint_t sum(0,std::max(rank(),other.rank())+1,radix);
        sum.digits.assign(digits.begin(),digits.end());
        sum += other;
        return sum;
    }

    bigint_t operator-(const bigint_t &other) const {
        if(radix != other.radix) {
            return operator-(other.convertToRadix(radix));
        }

        bigint_t sum(0,std::max(rank(),other.rank())+1,radix);
        sum.digits.assign(digits.begin(),digits.end());
        sum -= other;
        return sum;
    }

    bigint_t operator<<(size_t shift) const {
        size_t new_size = rank()+shift;
        bigint_t result(0,new_size,radix);
        result.digits.resize(new_size);
        std::copy(digits.begin(),digits.end(),result.digits.begin()+shift);
        return result;
    }

    bigint_t operator>>(size_t shift) const {
        if(shift >= rank()) {
            return bigint_t(0,0,radix);
        }

        size_t new_size = rank() - shift;
        bigint_t result(0,new_size,radix);
        result.digits.resize(new_size);
        std::copy(digits.begin()+shift,digits.end(),result.digits.begin());
        return result;
    }

    bigint_t operator*(const bigint_t &other) const {
        if(radix != other.radix) {
            return operator*(other.convertToRadix(radix));
        }

        bigint_t total(0,rank()+other.rank()+1,radix);

        for(auto a=digits.begin();a!=digits.end();++a) {
            for(auto b=other.digits.begin();b!=other.digits.end();++b) {
                size_t rank = (a - digits.begin()) + (b - other.digits.begin());
                uint16_t p = (uint16_t)*a * *b;
                bigint_t product(p,rank,radix);
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
        bigint_t total(0,rank(),radix);

        uint64_t current = 0;

        for(auto i=digits.rbegin();i!=digits.rend();++i) {
            current = current*radix + *i;
            if(current >= n) {
                size_t rank = digits.rend() - i - 1;
                total += bigint_t(current/n,rank,radix);
                current %= n;
            }
        }

        return total;
    }

    bigint_t operator/(const bigint_t &other) const {
        if(radix != other.radix) {
            return operator/(other.convertToRadix(radix));
        }

        bigint_t total(0,rank(),radix);
        bigint_t current(*this);

        for(int i=0;current.compare(other) > 0;++i) {
            auto q = current.guess_divisor(other);
            bigint_t sub = other*bigint_t(q,0,radix);
            int rank_diff = std::max(0,(int)current.rank() - (int)sub.rank());
            bigint_t head = current >> rank_diff;

//            fprintf(stderr,"1cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                    current.toString(radix).c_str(),
//                    other.toString(radix).c_str(),
//                    q,
//                    rank_diff,
//                    head.toString(radix).c_str(),
//                    sub.toString(radix).c_str());

            for(;q && sub > head;--q) {
                sub -= other;
            }

//            fprintf(stderr,"2cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                    current.toString(radix).c_str(),
//                    other.toString(radix).c_str(),
//                    q,
//                    rank_diff,
//                    head.toString(radix).c_str(),
//                    sub.toString(radix).c_str());

            if(!q && rank_diff) {
                q = radix - 1;
                sub = other*bigint_t(q,0,radix);
                rank_diff = std::max(0,(int)current.rank() - (int)sub.rank());
                bigint_t head = current >> rank_diff;

//                fprintf(stderr,"3cur[%s] other[%s] q[%d] rank_diff[%d]\nhead[%s]\n sub[%s]\n",
//                        current.toString(radix).c_str(),
//                        other.toString(radix).c_str(),
//                        q,
//                        rank_diff,
//                        head.toString(radix).c_str(),
//                        sub.toString(radix).c_str());
            }

            current -= sub << rank_diff;

            total += bigint_t(q,rank_diff,radix);
        }

        int cmp = current.compare(other);
        if(cmp == 0) {
            return total + bigint_t(1,0,radix);
        }
        return total;
    }

    std::string toString(digit_t view_radix=10) const {
        const bigint_t &view = (radix == view_radix) ? *this : convertToRadix(view_radix);

        const char *single_digits = "0123456789ABCDEFGHIJKLMNOPQRSTYVWXYZabcdefghijklmnopqrstyvwxyz";
        const size_t digit_len = view_radix <= strlen(single_digits) ? 1 : 4;

        auto buffer = std::make_unique<char[]>(view.rank()*digit_len);
        char* out = buffer.get() + view.rank()*digit_len;
        for(auto n: view.digits) {
            if(digit_len == 1) {
                *--out = single_digits[n];
            } else {
                *--out = ']';
                *--out = single_digits[n & 0xF];
                *--out = single_digits[n >> 4];
                *--out = '[';
            }
        }

        return std::string(out,view.rank()*digit_len);
    }

    explicit operator bool() const {
        return rank() != 0;
    }

    bigint_t convertToRadix(digit_t new_radix) const {
        bigint_t result(0,ceil(rank()/(log(new_radix)/log(radix))),new_radix);
        bigint_t current(*this);

        for(bigint_t current=*this;current;current = current/new_radix) {
            result.digits.push_back(current%new_radix);
        }

        return result;
    }
};

#endif // BIGINT_H
