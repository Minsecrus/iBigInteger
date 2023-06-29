#include <iostream>
#include <ranges>
#include <vector>
#include <cstring>

class iBigInteger;

void operator+=(iBigInteger &lhs, iBigInteger &&rhs);

void operator+=(iBigInteger &lhs, const iBigInteger &rhs);

void operator-=(iBigInteger &lhs, iBigInteger &&rhs);

void operator-=(iBigInteger &lhs, const iBigInteger &rhs);

class iBigInteger {
    std::vector<unsigned> v;
    bool _flag;
public:

    [[nodiscard]] size_t size() const {
        return v.size();
    }

    iBigInteger() {
        v.emplace_back(0);
        _flag = true;
    }

    iBigInteger(int i) {
        v.emplace_back(i > 0 ? i : -i);
        if (i > 0) _flag = true;
        else _flag = false;
    }

    iBigInteger(unsigned i) {
        v.emplace_back(i);
        _flag = true;
    }

    iBigInteger(long long i) {
        if (i == 0) {
            v.emplace_back(0);
        } else {
            v.emplace_back(i & (unsigned) 0xffffffff);
            v.emplace_back(i >> 32);
        }
        if (i > 0) _flag = true;
        else _flag = false;
    }

    iBigInteger(unsigned long long i) {
        if (i == 0) {
            v.emplace_back(0);
        } else {
            v.emplace_back(i & (unsigned) 0xffffffff);
            v.emplace_back(i >> 32);
        }
        _flag = true;
    }

    iBigInteger(const iBigInteger &i) {
        v = i.v;
        _flag = i._flag;
    }


    iBigInteger(const std::string &s, int radix = 10) {
        if (s == std::string(""))iBigInteger(0);
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
        if (_flag) {
            for (int i = (int) stringLength - 1; i >= 0; i--) {
                if (basicString[i] >= '0' && basicString[i] <= '9') {
                    dealtString[dealtLength++] = basicString[i];
                }
                if (basicString[i] >= 'a' && basicString[i] <= 'f') {
                    dealtString[dealtLength++] = (char) (basicString[i] - 'a' + ':');
                }
            }
        } else {
            for (int i = (int) stringLength - 1; i >= 1; i--) {
                if (basicString[i] >= '0' && basicString[i] <= '9') {
                    dealtString[dealtLength++] = basicString[i];
                }
                if (basicString[i] >= 'a' && basicString[i] <= 'f') {
                    dealtString[dealtLength++] = (char) (basicString[i] - 'a' + ':');
                }
            }
        }
        dealtString[dealtLength] = '\0';
        v.emplace_back(0);

        int alreadyInputDigit = 0;
        while (!check0(dealtString, dealtLength)) {
            v[alreadyInputDigit / 32] += (getEvenOrOdd(dealtString) ? 0 : 1) << (alreadyInputDigit % 32);
            alreadyInputDigit++;
            divideBy2(dealtString, radix);
            if (alreadyInputDigit % 32 == 0 && !check0(dealtString, dealtLength)) {
                v.emplace_back(0);
            }
        }
    }

    iBigInteger operator+(const iBigInteger &b) {
        if (isZero())return b;
        if (b.isZero())return *this;
        iBigInteger c;
        if (this->_flag != b._flag) {
            return *this - (-b);
        }
        long long temp;
        long long carry = 0;
        size_t max = std::max(size(), b.size());
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

    void setFlag(bool flag) {
        _flag = flag;
    }


    iBigInteger operator-(const iBigInteger &b) {
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
        size_t max = std::max(size(), b.size());
        for (int i = 0; i < max; i++) {
            temp = -carry + (v.size() > i ? v[i] : 0) - (b.v.size() > i ? b.v[i] : 0);
            carry = 0;
            if (temp < 0) {
                temp += (2ull << 31);
                carry = 1;
            }
            if (c.v.capacity() <= i) {
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

    iBigInteger operator*(const iBigInteger &i) {

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

    iBigInteger pupilMultiply(const iBigInteger &i) {
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

    iBigInteger karatsubaMultiply(const iBigInteger &i) {


        return {""};
    }

    iBigInteger NTTMultiply(const iBigInteger &i) {
        return {""};
    }

    iBigInteger operator/(const iBigInteger &i) {
        return {0};
    }

    iBigInteger operator/(int n) {
        return {0};
    }

    iBigInteger operator%(const iBigInteger &i) {
        return {0};
    }

    iBigInteger operator%(int n) {
        return {0};
    }

    iBigInteger operator^(int n) {
        return {0};
    }

    iBigInteger operator^(const iBigInteger &i) {
        return {0};
    }

    iBigInteger operator-() const {
        return negative(*this);
    }

    iBigInteger operator<<(int n) {
        return moveLeft(n);
    }

    iBigInteger operator>>(int n) {
        return moveRight(n);
    }


public:

    void delete0() {
        // remove zeros in the end of v
        while (v.size() > 1 && v.back() == 0) {
            v.pop_back();
        }
    }

    iBigInteger moveLeft(int n) {
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

    iBigInteger moveRight(int n) {
        if (isZero())return *this;
        iBigInteger c;
        c.v.pop_back();
        int cnt = n;
        for (unsigned int &i: v) {
            if (cnt > 0) {
                cnt -= 1;
            } else {
                c.v.emplace_back(i);
            }
        }
        return c;
    }

    static bool check0(const char *str, int length) {
        for (int i = length - 1; i >= 0; i--) {
            if (str[i] != '0') {
                return false;
            }
        }
        return true;
    }

    static bool getEvenOrOdd(const char *str) {
        return (str[0] - '0') % 2 == 0;
    }

    static void divideBy2(char *str, int radix) {
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

    void printEachNumber() const {
        int n = 0;
        std::cout << (_flag ? "==> positive" : "==> negative") << std::endl;
        for (auto i: v) {
            for (int j = 0; j < 31; j++) {
                std::cout << ((i & 1 << j) >> j) << " ";
            }
            std::cout << " num[" << ++n << "] is: " << i << std::endl;
        }
    }

    static iBigInteger negative(const iBigInteger &i) {
        iBigInteger c(i);
        c._flag = !c._flag;
        return c;
    }

    iBigInteger absoluteValue() {
        iBigInteger c(*this);
        c._flag = true;
        return c;
    }

    iBigInteger sqrt() {
        return {0};
    }

    static iBigInteger from1MovingLeft(int n) {
        iBigInteger i;
        for (int j = 1; j < n; j++) {
            i.v.emplace_back(0);
        }
        i.v.emplace_back(1);
        return i;
    }

    [[nodiscard]]bool isZero() const {
        return this->v.size() == 1 && this->v[0] == 0;
    }

    [[nodiscard]]std::vector<bool> toVector() const {
        std::vector<bool> bv;
        for (auto i: v) {
            for (int a = 0; a < 32; ++a) {
                bv.emplace_back((i & (1 << a)) >> a);
            }
        }
        return bv;
    }

    static void add1(char *str) {
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

    static void multiplyBy2(char *str) {
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

    [[nodiscard]]std::string toString() const {
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

public:
    static iBigInteger fillBits(long long n) {
        iBigInteger i;
        i.v.pop_back();
        for (long long cnt = 0; cnt < n / 32; cnt++) {
            i.v.emplace_back(4294967295);
        }
        i.v.emplace_back((1 << (n % 32)) - 1);
        return i;
    }

    friend std::ostream &operator<<(std::ostream &os, const iBigInteger &i) {
        os << i.toString();
        return os;
    }

    friend auto operator<=>(const iBigInteger &lhs, const iBigInteger &rhs);

    friend bool operator==(const iBigInteger &lhs, const iBigInteger &rhs);
};


void operator+=(iBigInteger &lhs, const iBigInteger &rhs) {
    lhs = lhs + rhs;
}

void operator+=(iBigInteger &lhs, iBigInteger &&rhs) {
    lhs = lhs + rhs;
}

void operator-=(iBigInteger &lhs, const iBigInteger &rhs) {
    lhs = lhs - rhs;
}

void operator-=(iBigInteger &lhs, iBigInteger &&rhs) {
    lhs = lhs - rhs;
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
