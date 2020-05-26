#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

/*--------------------------------------------------------------------------------------*/
// Dual

namespace dual {

using Float = double;

struct Dual {
    Float v, d; // [v]alue and [d]erivative
    Dual(Float v, Float d = 0.f): v(v), d(d) {}
};

Dual operator+(const Dual &a) {
    return a;
}

Dual operator-(const Dual &a) {
    return Dual{
        -a.v,
        -a.d
    };
}

Dual operator+(const Dual &a, const Dual &b) {
    return Dual{
        a.v + b.v,
        a.d + b.d
    };
}

Dual operator-(const Dual &a, const Dual &b) {
    return Dual{
        a.v - b.v,
        a.d - b.d
    };
}

Dual operator*(const Dual &a, const Dual &b) {
    return Dual{
        a.v * b.v,
        a.v * b.d + a.d * b.v
    };
}

Dual operator/(const Dual &a, const Dual &b) {
    return Dual{
        a.v / b.v,
        (a.d * b.v - a.v * b.d) / (b.v * b.v)
    };
}

Dual sin(const Dual &a) {
    return Dual{
        std::sin(a.v),
        std::cos(a.v) * a.d
    };
}

Dual asin(const Dual &a) {
    return Dual{
        std::asin(a.v),
        1.f / std::sqrt(1.f - a.v * a.v) * a.d
    };
}

Dual cos(const Dual &a) {
    return Dual{
        std::cos(a.v),
        -std::sin(a.v) * a.d
    };
}

Dual acos(const Dual &a) {
    return Dual{
        std::acos(a.v),
        -1.f / std::sqrt(1.f - a.v * a.v) * a.d
    };
}

Dual tan(const Dual &a) {
    return Dual{
        std::tan(a.v),
        1.f / std::cos(a.v) * a.d
    };
}

Dual atan(const Dual &a) {
    return Dual{
        std::atan(a.v),
        1.f / (1.f + a.v * a.v) * a.d
    };
}

Dual atan2(const Dual &a, const Dual &b) {
    Dual ratio = a / b;
    return Dual{
        std::atan2(a.v, b.v),
        1.f / (1.f + ratio.v * ratio.v) * ratio.d
    };
}

Dual pow(const Dual &a, const Dual &b) {
    Float p = std::pow(a.v, b.v);
    return Dual{
        p,
        b.v * std::pow(a.v, b.v - 1.f) * a.d + p * std::log(a.v) * b.d
    };
}

Dual exp(const Dual &a) {
    Float e = std::exp(a.v);
    return Dual{
        e,
        e * a.d
    };
}

Dual log(const Dual &a) {
    return Dual{
        std::log(a.v),
        1.f / a.v * a.d
    };
}

Dual abs(const Dual &a) {
    return Dual{
        std::abs(a.v),
        a.v > 0.f ? a.d : a.v < 0.f ? -a.d : 0.f
    };
}

Dual AsVar(Dual a) {
    if(a.d == 0.f)
        a.d = 1.f;
    return a;
}

}

/*--------------------------------------------------------------------------------------*/
// tests

int main() {
    using namespace dual;

    Dual a{2.f}, b{3.f};

    /*----------------------------------------------------------------------------------*/
    // F0

    auto F0 = [](const auto &a, const auto &b) {
        return a + b;
        // derivative to a: 1
        // derivative to b: 1
        // derivative     : 2
    };

    Dual f0 = F0(a, b);
    assert(f0.v == 5.f);
    assert(f0.d == 0.f);

    Dual f0a = F0(AsVar(a), b);
    assert(f0a.v == 5.f);
    assert(f0a.d == 1.f);

    Dual f0b = F0(a, AsVar(b));
    assert(f0b.v == 5.f);
    assert(f0b.d == 1.f);

    Dual f0ab = F0(AsVar(a), AsVar(b));
    assert(f0ab.v == 5.f);
    assert(f0ab.d == 2.f);

    /*----------------------------------------------------------------------------------*/
    // F1

    auto F1 = [](const auto &a, const auto &b) {
        return (a + b) * (a - b);
        // derivative to a: 2a
        // derivative to b: -2b
        // derivative     : 2a - 2b
    };

    Dual f1 = F1(a, b);
    assert(f1.v == -5.f);
    assert(f1.d == 0.f);

    Dual f1a = F1(AsVar(a), b);
    assert(f1a.v == -5.f);
    assert(f1a.d == 4.f);

    Dual f1b = F1(a, AsVar(b));
    assert(f1b.v == -5.f);
    assert(f1b.d == -6.f);

    Dual f1ab = F1(AsVar(a), AsVar(b));
    assert(f1ab.v == -5.f);
    assert(f1ab.d == -2.f);

    /*----------------------------------------------------------------------------------*/
    // F2

    auto F2 = [](const auto &a, const auto &b) {
        return (a + b) / (a - b);
        // derivative to a: -2b / (a-b)^2
        // derivative to b: 2a / (a-b)^2
        // derivative     : 2 / (a-b)
    };

    Dual f2a = F2(AsVar(a), b);
    assert(f2a.v == -5.f);
    assert(f2a.d == -6.f);

    Dual f2b = F2(a, AsVar(b));
    assert(f2b.v == -5.f);
    assert(f2b.d == 4.f);

    Dual f2ab = F2(AsVar(a), AsVar(b));
    assert(f2ab.v == -5.f);
    assert(f2ab.d == -2.f);

    /*----------------------------------------------------------------------------------*/
    // F3

    auto F3 = [](const auto &a, const auto &b) {
        return sin(tan(a * b)) * exp(cos(a + b))
            / (cos(exp(a / b)) * tan(sin(a - b)));
        // derivative to a: ...
        // derivative to b: ...
        // derivative     : ...
    };

    Dual f3a = F3(AsVar(a), b);
    cout << "f3a.d  = " << f3a.d << endl;

    Dual f3b = F3(a, AsVar(b));
    cout << "f3b.d  = " << f3b.d << endl;

    Dual f3ab = F3(AsVar(a), AsVar(b));
    cout << "f3ab.d = " << f3ab.d << endl;

    if constexpr(std::is_same_v<Float, float>) {
        assert(std::abs(f3a.d + f3b.d - f3ab.d) < 1e-5f);
        assert(std::abs(f3a.d + f3b.d - f3ab.d) > 1e-6f);
    } else {
        assert(std::abs(f3a.d + f3b.d - f3ab.d) < 1e-14f);
        assert(std::abs(f3a.d + f3b.d - f3ab.d) > 1e-15f);
    }

    /*----------------------------------------------------------------------------------*/
    // F4

    auto F4 = [](auto a) {
        for(int i = 0; i < 3; ++i)
            a = exp(a);
        return a;
        // derivative: exp(exp(exp(a))) * exp(exp(a)) * exp(a)
    };

    Dual c{1.f};
    Dual f4 = F4(AsVar(c));
    assert(f4.d == exp(exp(exp(c.v))) * exp(exp(c.v)) * exp(c.v));

    return 0;
}
