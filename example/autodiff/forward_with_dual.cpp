#include <iostream>
#include <cassert>
#include <cmath>
#include <valarray>
using namespace std;

/*--------------------------------------------------------------------------------------*/
// Dual

namespace dual {

using Float = float;

struct Dual {
    Float v; // [v]alue
    std::valarray<Float> d; // [d]erivatives

    template<typename ...Ts>
    Dual(Float v, Ts ...d) : v{v}, d{d...} {}

    // C++20
    // Dual(Float v, std::floating_point auto... d) : v{v}, d{static_cast<Float>(d)...} {}
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
        a.v > 0.f ? a.d : a.v < 0.f ? -a.d : valarray<Float>{0.f}
    };
}

}

/*--------------------------------------------------------------------------------------*/
// tests

int main() {
    using namespace dual;

    // single variable, total derivative
    Dual a{2.f, 1.f}, b{3.f, 1.f};
    // multiple variables, partial derivatives
    Dual a_p{2.f, 1.f, 0.f}, b_p{3.f, 0.f, 1.f};

    /*----------------------------------------------------------------------------------*/
    // F0

    auto F0 = [](const auto &a, const auto &b) {
        return a + b;
        // derivative to a: 1
        // derivative to b: 1
        // derivative     : 2
    };

    Dual f0_ab = F0(a, b);
    assert(f0_ab.v == 5.f);
    assert(f0_ab.d[0] == 2.f);

    Dual f0ab_p = F0(a_p, b_p);
    assert(f0ab_p.v == 5.f);
    assert(f0ab_p.d[0] == 1.f);
    assert(f0ab_p.d[1] == 1.f);

    /*----------------------------------------------------------------------------------*/
    // F1

    auto F1 = [](const auto &a, const auto &b) {
        return (a + b) * (a - b);
        // derivative to a: 2a
        // derivative to b: -2b
        // derivative     : 2a - 2b
    };

    Dual f1_ab = F1(a, b);
    assert(f1_ab.v == -5.f);
    assert(f1_ab.d[0] == -2.f);

    Dual f1_ab_p = F1(a_p, b_p);
    assert(f1_ab_p.v == -5.f);
    assert(f1_ab_p.d[0] == 4.f);
    assert(f1_ab_p.d[1] == -6.f);

    /*----------------------------------------------------------------------------------*/
    // F2

    auto F2 = [](const auto &a, const auto &b) {
        return (a + b) / (a - b);
        // derivative to a: -2b / (a-b)^2
        // derivative to b: 2a / (a-b)^2
        // derivative     : 2 / (a-b)
    };

    Dual f2_ab = F2(a, b);
    assert(f2_ab.v == -5.f);
    assert(f2_ab.d[0] == -2.f);

    Dual f2_ab_p = F2(a_p, b_p);
    assert(f2_ab_p.v == -5.f);
    assert(f2_ab_p.d[0] == -6.f);
    assert(f2_ab_p.d[1] == 4.f);

    /*----------------------------------------------------------------------------------*/
    // F3

    auto F3 = [](const auto &a, const auto &b) {
        return sin(tan(a * b)) * exp(cos(a + b))
            / (cos(exp(a / b)) * tan(sin(a - b)));
        // derivative to a: ...
        // derivative to b: ...
        // derivative     : ...
    };

    Dual f3_ab = F3(a, b);
    cout << "f3_ab.d      = " << f3_ab.d[0] << endl;

    Dual f3_ab_p = F3(a_p, b_p);
    cout << "f3_ab_p.d[0] = " << f3_ab_p.d[0] << endl;
    cout << "f3_ab_p.d[1] = " << f3_ab_p.d[1] << endl;

    if constexpr(std::is_same_v<Float, float>) {
        assert(std::abs(f3_ab_p.d.sum() - f3_ab.d[0]) < 1e-5f);
        assert(std::abs(f3_ab_p.d.sum() - f3_ab.d[0]) > 1e-6f);
    } else {
        assert(std::abs(f3_ab_p.d.sum() - f3_ab.d[0]) < 1e-14f);
        assert(std::abs(f3_ab_p.d.sum() - f3_ab.d[0]) > 1e-15f);
    }

    /*----------------------------------------------------------------------------------*/
    // F4

    auto F4 = [](auto a) {
        for(int i = 0; i < 3; ++i)
            a = exp(a);
        return a;
        // derivative: exp(exp(exp(a))) * exp(exp(a)) * exp(a)
    };

    Dual c{1.f, 1.f};
    Dual f4 = F4(c);
    assert(f4.d[0] == exp(exp(exp(c.v))) * exp(exp(c.v)) * exp(c.v));

    return 0;
}
