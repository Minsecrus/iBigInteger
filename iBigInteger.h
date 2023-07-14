#ifndef IBIGINTEGER_DEMO_IBIGINTEGER_H
#define IBIGINTEGER_DEMO_IBIGINTEGER_H

#include <vector>
#include <string>

class iBigInteger {
    std::vector<unsigned> v;
    bool _flag;
public:
    iBigInteger();

    iBigInteger(int i);

    iBigInteger(unsigned i);

    iBigInteger(long long i);

    iBigInteger(unsigned long long i);

    iBigInteger(const iBigInteger &i);

    iBigInteger(const std::string &s, int radix = 10);


    iBigInteger operator+(const iBigInteger &b)const;

    iBigInteger operator-(const iBigInteger &b)const;

    iBigInteger operator*(const iBigInteger &i);

    iBigInteger operator%(const iBigInteger &i);

    iBigInteger operator-() const;

    iBigInteger operator<<(int n);

    iBigInteger operator>>(int n);

    iBigInteger pupilMultiply(const iBigInteger &i);

    iBigInteger karatsubaMultiply(const iBigInteger &i);

    iBigInteger NTTMultiply(const iBigInteger &i);


    void printEachNumber() const;

    [[nodiscard]] size_t size()const;

private:

    friend auto operator<=>(const iBigInteger &lhs, const iBigInteger &rhs);

    friend bool operator==(const iBigInteger &lhs, const iBigInteger &rhs);

    void setFlag(bool flag);

    static bool accordWithRadix(int c, int radix);

    void delete0();

    iBigInteger moveLeft(int n)const;

    iBigInteger moveRight(int n)const;

    static bool check0(const char *str, int length);

    static bool getEvenOrOdd(const char *str);

    static void divideBy2(char *str, int radix);

    static iBigInteger negative(const iBigInteger &i);

    iBigInteger absoluteValue();

    static iBigInteger from1MovingLeft(int n);

    static iBigInteger fillBits(long long n);


    [[nodiscard]]bool isZero() const;

    [[nodiscard]]std::vector<bool> toVector() const;

    [[nodiscard]]std::string toString() const;


    static void add1(char *str);

    static void multiplyBy2(char *str);


    friend std::ostream &operator<<(std::ostream &os, const iBigInteger &i);
};

void operator+=(iBigInteger &lhs, iBigInteger &&rhs);

void operator+=(iBigInteger &lhs, const iBigInteger &rhs);

void operator-=(iBigInteger &lhs, iBigInteger &&rhs);

void operator-=(iBigInteger &lhs, const iBigInteger &rhs);

void operator++(iBigInteger &lhs);

void operator--(iBigInteger &lhs);
#endif //IBIGINTEGER_DEMO_IBIGINTEGER_H
