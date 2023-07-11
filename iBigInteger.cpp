#include <cstring>
#include <iostream>
#include <ranges>
#include "iBigInteger.h"

iBigInteger::iBigInteger() {
    v.emplace_back(0);
    _flag = true;
}

iBigInteger::iBigInteger(int i) {
    v.emplace_back(i > 0 ? i : -i);
    if (i > 0) _flag = true;
    else _flag = false;
}

iBigInteger::iBigInteger(unsigned i) {
    v.emplace_back(i);
    _flag = true;
}

iBigInteger::iBigInteger(long long i) {
    if (i == 0) {
        v.emplace_back(0);
    }
    if (i > 0) {
        v.emplace_back(i & (unsigned) 0xffffffff);
        int n = (int) (i >> 32);
        if (n > 0)v.emplace_back(n);
        _flag = true;
    } else {
        long long temp = -i;
        v.emplace_back(temp & (unsigned) 0xffffffff);
        int n = (int) (temp >> 32);
        if (n > 0)v.emplace_back(n);
        _flag = false;
    }
}

iBigInteger::iBigInteger(unsigned long long i) {
    if (i == 0) {
        v.emplace_back(0);
    } else {
        v.emplace_back(i & (unsigned) 0xffffffff);
        if ((i >> 32) > 0)v.emplace_back(i >> 32);
    }
    _flag = true;
}

iBigInteger::iBigInteger(const iBigInteger &i) {
    v = i.v;
    _flag = i._flag;
}

iBigInteger::iBigInteger(const std::string &s, int radix) {
    if (radix < 2 || radix > 16) {
        std::cout << "?";
        v.emplace_back(0);
        _flag = true;
        return;
    }
    const char *basicString = s.c_str();
    size_t stringLength = std::strlen(basicString);
    char firstChar = basicString[0];
    if (firstChar == '-') {
        _flag = false;
    } else {
        _flag = true;
    }
    char *dealtString = new char[stringLength];
    for (int i = 0; i < stringLength; i++)
        dealtString[i] = 0;
    int dealtLength = 0;
    for (int i = (int) stringLength - 1; i >= 0; i--) {
        if (accordWithRadix(basicString[i], radix)) {
            if (basicString[i] >= '0' && basicString[i] <= '9') {
                dealtString[dealtLength++] = basicString[i];
            }
            if (basicString[i] >= 'a' && basicString[i] <= 'z') {
                dealtString[dealtLength++] = (char) (basicString[i] - 'a' + ':');
            }
        }
    }
    dealtString[dealtLength] = '\0';
    v.emplace_back(0);

    long long alreadyInputDigit = 0;
    while (!check0(dealtString, dealtLength)) {
        v[alreadyInputDigit / 32] += (getEvenOrOdd(dealtString) ? 0 : 1) << (alreadyInputDigit % 32);
        alreadyInputDigit++;
        divideBy2(dealtString, radix);
        if (alreadyInputDigit % 32 == 0 && !check0(dealtString, dealtLength)) {
            v.emplace_back(0);
        }
    }
}

iBigInteger iBigInteger::operator+(const iBigInteger &b) const {
    if (isZero())return b;
    if (b.isZero())return *this;
    iBigInteger c;
    if (this->_flag != b._flag) {
        return *this - (-b);
    }
    long long temp;
    long long carry = 0;
    size_t max = std::max(this->v.size(), b.v.size());
    for (int i = 0; i < max; i++) {
        temp = carry + (v.size() > i ? v[i] : 0) + (b.v.size() > i ? b.v[i] : 0);
        carry = 0;
        if (temp > (unsigned) (2 << 31) - 1) {
            temp -= (2 << 31);
            carry = 1;
        }

        if (c.v.size() <= i) {
            c.v.emplace_back(0);

        }
        c.v[i] = temp;
    }
    if (carry) {
        c.v.emplace_back(carry);
    }
    c._flag = _flag;
    return c;
}

iBigInteger iBigInteger::operator-(const iBigInteger &b) const {
    if (isZero())return -b;
    if (b.isZero())return *this;
    iBigInteger c;
    if (this->_flag != b._flag) {
        return *this + (-b);
    }
    bool flag;
    if (this->_flag) {
        flag = true;
    } else flag = false;
    long long temp;
    long long carry = 0;
    size_t max = std::max(this->v.size(), b.v.size());
    for (int i = 0; i < max; i++) {
        temp = -carry + (v.size() > i ? v[i] : 0) - (b.v.size() > i ? b.v[i] : 0);
        carry = 0;
        if (temp < 0) {
            temp += (2ull << 31);
            carry = 1;
        }
        if (c.v.size() <= i) {
            c.v.emplace_back(0);
        }
        c.v[i] = temp;
    }
    // remove zeros in the end of c.v
    while (c.v.size() > 1 && c.v.back() == 0) {
        c.v.pop_back();
    }

    if (carry != 0) {
        iBigInteger result = from1MovingLeft((int) max) - c;
        result.setFlag(!flag);
        return result;
    }
    c._flag = flag;
    return c;
}

iBigInteger iBigInteger::operator*(const iBigInteger &i) {

    if (isZero() || i.isZero())return {0};
    bool resultFlag = !this->_flag ^ i._flag;
    if (this->v.size() < 32 && i.v.size() < 32) {
#ifdef TEST
        std::cout << "<=> pupilMultiply" << std::endl;
#endif
        iBigInteger result = pupilMultiply(i);
#ifdef TEST
        std::cout << result << std::endl;
#endif
        result.setFlag(resultFlag);
        return result;
    } else if (this->v.size() < 256 && i.v.size() < 256) {
#ifdef TEST
        std::cout << "<=> KaratsubaMultiply" << std::endl;
#endif
        iBigInteger result = karatsubaMultiply(i);
        result.setFlag(resultFlag);
        return result;
    }

    return {""};
}
iBigInteger iBigInteger::operator%(const iBigInteger &i) {
    iBigInteger c=*this;
    if(c<i) return c;
    while(c>=i) c-=i;
    return c;
}
iBigInteger iBigInteger::operator-() const {
    return negative(*this);
}

iBigInteger iBigInteger::operator<<(int n) {
    return moveLeft(n);
}

iBigInteger iBigInteger::operator>>(int n) {
    return moveRight(n);
}

void iBigInteger::setFlag(bool flag) {
    _flag = flag;
}

iBigInteger iBigInteger::pupilMultiply(const iBigInteger &i) {
    iBigInteger c;
    for (unsigned int j: std::ranges::reverse_view(v)) {
        c = c.moveLeft(1);
        iBigInteger temp;
        for (unsigned int k: std::ranges::reverse_view(i.v)) {
            temp = temp.moveLeft(1);
            temp += {j * 1ull * k};
        }
        c += temp;
    }
    c.delete0();
    return c;
}

iBigInteger iBigInteger::karatsubaMultiply(const iBigInteger &i) {
    int n = std::max((int) this->v.size() / 2, (int) i.v.size() / 2);
    iBigInteger p = this->moveRight(n), q, s;

    if (p == 0) q = (*this);
    else q = (*this) - p.moveLeft(n);

    iBigInteger r = i.moveRight(n);

    if (r == 0) s = i;
    else s = i - r.moveLeft(n);
    /*
    //    p  q
    //    r  s
    //---------
    //   ps qs
    //pr qr
    // (p+q)(r+s)=pr+qr+ps+qs
    // ps+qr=(p+q)(r+s)-pr-qs
    */

    iBigInteger q_mul_s = q * s;
    iBigInteger r_mul_p = r * p;

    iBigInteger p_add_q = p + q;
    iBigInteger r_add_s = r + s;

    iBigInteger p_add_q_mul_r_add_s = p_add_q * r_add_s;

    return r_mul_p.moveLeft(2 * n) + (p_add_q_mul_r_add_s - r_mul_p - q_mul_s).moveLeft(n) + q_mul_s;

}

iBigInteger iBigInteger::NTTMultiply(const iBigInteger &i) {
    return {};
}

void iBigInteger::delete0() {
// remove zeros in the end of v
    while (v.size() > 1 && v.back() == 0) {
        v.pop_back();
    }
}

iBigInteger iBigInteger::moveLeft(int n) const {
    if (isZero())return *this;
    iBigInteger c;
    for (int i = 1; i < n; i++) {
        c.v.emplace_back(0);
    }
    for (auto i: v) {
        c.v.emplace_back(i);
    }
    c._flag = _flag;
    return c;
}

iBigInteger iBigInteger::moveRight(int n) const {
    if (isZero())return *this;
    bool movedInto = false;
    iBigInteger c;
    c.v.pop_back();
    int cnt = n;
    for (const unsigned int &i: v) {
        if (cnt > 0) {
            cnt -= 1;
        } else {
            movedInto = true;
            c.v.emplace_back(i);
        }
    }
    if (!movedInto) {
        c.v.emplace_back(0);
    }
    return c;
}

bool iBigInteger::check0(const char *str, int length) {
    for (int i = length - 1; i >= 0; i--) {
        if (str[i] != '0') {
            return false;
        }
    }
    return true;
}

bool iBigInteger::getEvenOrOdd(const char *str) {
    return str[0] % 2 == 0;
}

void iBigInteger::divideBy2(char *str, int radix) {
    bool carry = false;
    for (int i = (int) strlen(str) - 1; i >= 0; --i) {
        int temp = str[i] - '0';
        str[i] = (char) ((str[i] - '0' + (carry ? radix : 0)) / 2 + '0');
        carry = false;
        if (temp % 2 == 1) {
            carry = true;
        }
    }
}

void iBigInteger::printEachNumber() const {
    int n = 0;
    std::cout << (_flag ? "==> positive" : "==> negative") << std::endl;
    for (auto i: v) {
        for (int j = 0; j < 31; j++) {
            std::cout << ((i & 1 << j) >> j) << " ";
        }
        std::cout << " num[" << ++n << "] is: " << i << std::endl;
    }
}

iBigInteger iBigInteger::negative(const iBigInteger &i) {
    iBigInteger c(i);
    c._flag = !c._flag;
    return c;
}

iBigInteger iBigInteger::absoluteValue() {
    iBigInteger c(*this);
    c._flag = true;
    return c;
}

iBigInteger iBigInteger::from1MovingLeft(int n) {
    iBigInteger i;
    for (int j = 1; j < n; j++) {
        i.v.emplace_back(0);
    }
    i.v.emplace_back(1);
    return i;
}

iBigInteger iBigInteger::fillBits(long long int n) {
    iBigInteger i;
    i.v.pop_back();
    for (long long cnt = 0; cnt < n / 32; cnt++) {
        i.v.emplace_back(4294967295);
    }
    i.v.emplace_back((1 << (n % 32)) - 1);
    return i;
}

size_t iBigInteger::size() const {
    return this->v.size();
}

bool iBigInteger::accordWithRadix(int c, int radix) {
    if (radix <= 10) {
        return c >= '0' && c < ('0' + radix);
    } else if (radix <= 36) {
        return c >= '0' && c <= '9' || c >= 'a' && c < ('a' + radix - 10);
    }
    return false;
}

bool iBigInteger::isZero() const {
    return this->v.size() == 1 && this->v[0] == 0;
}

std::vector<bool> iBigInteger::toVector() const {
    std::vector<bool> bv;
    for (auto i: v) {
        for (int a = 0; a < 32; ++a) {
            bv.emplace_back((i & (1 << a)) >> a);
        }
    }
    return bv;
}

std::string iBigInteger::toString() const {
    char str[v.size() * 11 + 1];
    // make each char in str is '0'
    for (int i = 0; i < v.size() * 11 + 1; ++i) {
        str[i] = '\0';
    }
    auto vec = toVector();
    std::reverse(vec.begin(), vec.end());
    for (auto i: vec) {
        multiplyBy2(str);
        if (i) {
            add1(str);
        }
    }
    std::string s{str};
    if (s == std::string(""))s = "0";
    if (!_flag)s += "-";
    std::reverse(s.begin(), s.end());
    return s;
}

void iBigInteger::add1(char *str) {
    for (int i = 0; i <= strlen(str); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            str[i] += 1;
            break;
        } else if (str[i] == '\0') {
            str[i] = '1';
            break;
        } else if (str[i] == '9') {
            str[i] = '0';
        }
    }
}

void iBigInteger::multiplyBy2(char *str) {
    bool carry = false;
    for (int i = 0; i <= strlen(str); ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            str[i] = (char) ((str[i] - '0') * 2 + (carry ? 1 : 0) + '0');
            carry = false;
            if (str[i] > '9') {
                str[i] -= 10;
                carry = true;
            }
        }
        if (str[i] == '\0') {
            if (carry) {
                str[i] = '1';
            }
            break;
        }
    }
}

std::ostream &operator<<(std::ostream &os, const iBigInteger &i) {
    os << i.toString();
    return os;
}

auto operator<=>(const iBigInteger &lhs, const iBigInteger &rhs) {
    if (lhs.isZero() && rhs.isZero())return std::strong_ordering::equal;
    if (lhs._flag && !rhs._flag)return std::strong_ordering::greater;
    if (!lhs._flag && rhs._flag)return std::strong_ordering::less;
    if (!lhs._flag) {
        if (lhs.size() < rhs.size())return std::strong_ordering::less;
        if (lhs.size() > rhs.size())return std::strong_ordering::greater;
        for (auto k = (long long) lhs.size() - 1; k >= 0; --k) {
            if (lhs.v[k] < rhs.v[k])return std::strong_ordering::less;
            if (lhs.v[k] > rhs.v[k])return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    } else {
        if (lhs.size() > rhs.size())return std::strong_ordering::less;
        if (lhs.size() < rhs.size())return std::strong_ordering::greater;
        for (auto k = (long long) lhs.size() - 1; k >= 0; --k) {
            if (lhs.v[k] < rhs.v[k])return std::strong_ordering::greater;
            if (lhs.v[k] > rhs.v[k])return std::strong_ordering::less;
        }
        return std::strong_ordering::equal;
    }
}

bool operator==(const iBigInteger &lhs, const iBigInteger &rhs) {
    return (lhs <=> rhs) == std::strong_ordering::equal;
}

void operator+=(iBigInteger &lhs, iBigInteger &&rhs) {
    lhs = lhs + rhs;
}

void operator+=(iBigInteger &lhs, const iBigInteger &rhs) {
    lhs = lhs + rhs;
}

void operator-=(iBigInteger &lhs, iBigInteger &&rhs) {
    lhs = lhs - rhs;
}

void operator-=(iBigInteger &lhs, const iBigInteger &rhs) {
    lhs = lhs - rhs;
}

void operator++(iBigInteger &lhs) {
    lhs = lhs + iBigInteger(1);
}

void operator--(iBigInteger &lhs) {
    lhs = lhs - iBigInteger(1);
}