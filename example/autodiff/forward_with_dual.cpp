#include <iostream>
#include <cassert>
#include <cmath>
#include <valarray>

/*------------------------------------------------------------------------------------------------*/
// Dual

namespace dual {

template<typename T, size_t N>
struct Dual {
    T v; // [v]alue
    std::valarray<T> d = std::valarray<T>(T{0.f}, N); // [d]erivatives

    Dual(T v): v{v} {}

    template<typename S>
    Dual(S v) : v{v} {}

    Dual(T v, std::valarray<T> d): v{v} {
        this->d[std::slice(0, N, 1)] = d; // Use `std::slice` to keep the size of `this->d`.
    }
};

using
    std::sin,
    std::asin,
    std::cos,
    std::acos,
    std::tan,
    std::atan,
    std::atan2,
    std::pow,
    std::sqrt,
    std::exp,
    std::log,
    std::abs;

template<typename T, size_t N>
bool operator<(const Dual<T, N> &a, const Dual<T, N> &b) {
    return a.v < b.v;
}

template<typename T, size_t N>
bool operator>(const Dual<T, N> &a, const Dual<T, N> &b) {
    return b < a;
}

template<typename T, size_t N>
Dual<T, N> operator+(const Dual<T, N> &a) {
    return a;
}

template<typename T, size_t N>
Dual<T, N> operator-(const Dual<T, N> &a) {
    return {
        -a.v,
        -a.d
    };
}

template<typename T, size_t N>
Dual<T, N> operator+(const Dual<T, N> &a, const Dual<T, N> &b) {
    return {
        a.v + b.v,
        a.d + b.d
    };
}

template<typename T, size_t N>
Dual<T, N> operator-(const Dual<T, N> &a, const Dual<T, N> &b) {
    return {
        a.v - b.v,
        a.d - b.d
    };
}

template<typename T, size_t N>
Dual<T, N> operator*(const Dual<T, N> &a, const Dual<T, N> &b) {
    return {
        a.v * b.v,
        b.v * a.d + a.v * b.d
    };
}

template<typename T, size_t N>
Dual<T, N> operator/(const Dual<T, N> &a, const Dual<T, N> &b) {
    return {
        a.v / b.v,
        (a.d * b.v - a.v * b.d) / (b.v * b.v)
    };
}

template<typename T, size_t N>
Dual<T, N> sin(const Dual<T, N> &a) {
    return {
        sin(a.v),
        cos(a.v) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> asin(const Dual<T, N> &a) {
    return {
        asin(a.v),
        T{1.f} / sqrt(T{1.f} - a.v * a.v) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> cos(const Dual<T, N> &a) {
    return {
        cos(a.v),
        -sin(a.v) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> acos(const Dual<T, N> &a) {
    return {
        acos(a.v),
        T{-1.f} / sqrt(T{1.f} - a.v * a.v) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> tan(const Dual<T, N> &a) {
    T cos_a = cos(a.v);
    return {
        tan(a.v),
        T{1.f} / (cos_a * cos_a) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> atan(const Dual<T, N> &a) {
    return {
        atan(a.v),
        T{1.f} / (T{1.f} + a.v * a.v) * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> atan2(const Dual<T, N> &a, const Dual<T, N> &b) {
    Dual<T, N> ratio = a / b;
    return {
        atan2(a.v, b.v),
        T{1.f} / (T{1.f} + ratio.v * ratio.v) * ratio.d
    };
}

template<typename T, size_t N>
Dual<T, N> pow(const Dual<T, N> &a, const Dual<T, N> &b) {
    T p = pow(a.v, b.v);
    return {
        p,
        b.v * pow(a.v, b.v - T{1.f}) * a.d + p * log(a.v) * b.d
    };
}

template<typename T, size_t N>
Dual<T, N> sqrt(const Dual<T, N> &a) {
    T s = sqrt(a.v);
    return {
        s,
        T{0.5f} / s * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> exp(const Dual<T, N> &a) {
    T e = exp(a.v);
    return {
        e,
        e * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> log(const Dual<T, N> &a) {
    return {
        log(a.v),
        T{1.f} / a.v * a.d
    };
}

template<typename T, size_t N>
Dual<T, N> abs(const Dual<T, N> &a) {
    return {
        abs(a.v),
        a.v > T{0.f} ? a.d : a.v < T{0.f} ? -a.d : std::valarray<T>{0.f}
    };
}

template<typename F>
auto D(const F &f) {
    return [f](auto ...a) {
        return f(Dual<decltype(a), 1>{a, {1.f}}...).d[0];
    };
};

template<unsigned N, typename F>
auto DN(const F &f) { // N-th derivative
    if constexpr(N == 0)
        return f;
    else
        return DN<N-1>(D(f));
}

}

/*------------------------------------------------------------------------------------------------*/
// tests

int main() {
    using namespace std;
    using namespace dual;
    using Float = float;
    // using Float = double;

    // single variable, total derivative
    Dual<Float, 1> a{2.f, {1.f}}, b{3.f, {1.f}};
    // multiple variables, partial derivatives
    Dual<Float, 2> a_p{2.f, {1.f, 0.f}}, b_p{3.f, {0.f, 1.f}};

    /*--------------------------------------------------------------------------------------------*/
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

    /*--------------------------------------------------------------------------------------------*/
    // F1

    auto F1 = [](const auto &a, const auto &b) {
        return (a + b) * (a - b);
        // derivative to a : 2a
        // derivative to b : -2b
        // total derivative: 2a - 2b
    };

    Dual f1_ab = F1(a, b);
    assert(f1_ab.v == -5.f);
    assert(f1_ab.d[0] == -2.f);

    Dual f1_ab_p = F1(a_p, b_p);
    assert(f1_ab_p.v == -5.f);
    assert(f1_ab_p.d[0] == 4.f);
    assert(f1_ab_p.d[1] == -6.f);

    /*--------------------------------------------------------------------------------------------*/
    // F2

    auto F2 = [](const auto &a, const auto &b) {
        return (a + b) / (a - b);
        // derivative to a : -2b / (a-b)^2
        // derivative to b : 2a / (a-b)^2
        // total derivative: 2 / (a-b)
    };

    Dual f2_ab = F2(a, b);
    assert(f2_ab.v == -5.f);
    assert(f2_ab.d[0] == -2.f);

    Dual f2_ab_p = F2(a_p, b_p);
    assert(f2_ab_p.v == -5.f);
    assert(f2_ab_p.d[0] == -6.f);
    assert(f2_ab_p.d[1] == 4.f);

    /*--------------------------------------------------------------------------------------------*/
    // F3, a complicated function using all operators

    auto F3 = [](const auto &a, const auto &b) {
        using T = decltype(a);
        return sin(atan(a * b)) * exp(acos(a + b))
            / (cos(log(a / b)) * tan(asin(a - b)))
            * sqrt(-a * -b + T{2.f}) * atan2(+a * T{7.f}, +b)
            / (pow(+a, -b / T{5.f}) * abs(-a + +b - T{3.f}))
            ;
        // derivative to a : ...
        // derivative to b : ...
        // total derivative: ...
    };

    Dual<Float, 1> c{0.2f, {1.f}}, d{0.3f, {1.f}};
    Dual<Float, 2> c_p{0.2f, {1.f, 0.f}}, d_p{0.3f, {0.f, 1.f}};

    Dual<Float, 2> g = c_p * Dual<Float, 2>{3.f, {0.f, 0.f}};
    Dual<Float, 2> h = c_p * Dual<Float, 2>{3.f, {0.f}};

    Dual f3_cd = F3(c, d);
    cout << "f3_cd.v      = " << f3_cd.v << endl;
    cout << "f3_cd.d[0]   = " << f3_cd.d[0] << endl;

    Dual f3_cd_p = F3(c_p, d_p);
    cout << "f3_cd_p.d[0] = " << f3_cd_p.d[0] << endl;
    cout << "f3_cd_p.d[1] = " << f3_cd_p.d[1] << endl;

    if constexpr(std::is_same_v<Float, float>) {
        assert(std::abs(f3_cd_p.d.sum() - f3_cd.d[0]) < 1e-6f);
        assert(std::abs(f3_cd_p.d.sum() - f3_cd.d[0]) > 1e-7f);
    } else {
        assert(std::abs(f3_cd_p.d.sum() - f3_cd.d[0]) < 1e-14f);
        assert(std::abs(f3_cd_p.d.sum() - f3_cd.d[0]) > 1e-15f);
    }

    /*--------------------------------------------------------------------------------------------*/
    // F4

    auto F4 = [](auto a) {
        for(int i = 0; i < 3; ++i)
            a = exp(a);
        return a;
        // total derivative: exp(exp(exp(a))) * exp(exp(a)) * exp(a)
    };

    Dual<Float, 1> e{1.f, {1.f}};
    Dual f4 = F4(e);
    assert(f4.d[0] == exp(exp(exp(e.v))) * exp(exp(e.v)) * exp(e.v));

    /*--------------------------------------------------------------------------------------------*/
    // F5 and F6, higher order derivatives

    auto F5 = [](const auto &a) {
        return a * a * a;
    };

    assert(DN<0>(F5)(4.f) == 64.f);
    assert(DN<1>(F5)(4.f) == 48.f);
    assert(DN<2>(F5)(4.f) == 24.f);
    assert(DN<3>(F5)(4.f) == 6.f);
    assert(DN<4>(F5)(4.f) == 0.f);
    assert(DN<5>(F5)(4.f) == 0.f);

    cout << endl;

    auto F6 = [](const auto &a, const auto &b) {
        return a * a * b * b;
    };

    assert(DN<0>(F6)(3.f, 5.f) == 225.f);
    assert(DN<1>(F6)(3.f, 5.f) == 240.f);
    assert(DN<2>(F6)(3.f, 5.f) == 188.f);
    assert(DN<3>(F6)(3.f, 5.f) == 96.f);
    assert(DN<4>(F6)(3.f, 5.f) == 24.f);
    assert(DN<5>(F6)(3.f, 5.f) == 0.f);
    assert(DN<6>(F6)(3.f, 5.f) == 0.f);

    /*--------------------------------------------------------------------------------------------*/
    // F3 again, higher order derivatives of a complicated function

    cout << "DN<0>(F3)(0.2f, 0.3f) = " << DN<0>(F3)(0.2f, 0.3f) << endl;
    cout << "DN<1>(F3)(0.2f, 0.3f) = " << DN<1>(F3)(0.2f, 0.3f) << endl;
    cout << "DN<2>(F3)(0.2f, 0.3f) = " << DN<2>(F3)(0.2f, 0.3f) << endl;
    cout << "DN<3>(F3)(0.2f, 0.3f) = " << DN<3>(F3)(0.2f, 0.3f) << endl;
    cout << "DN<4>(F3)(0.2f, 0.3f) = " << DN<4>(F3)(0.2f, 0.3f) << endl;
    cout << "DN<5>(F3)(0.2f, 0.3f) = " << DN<5>(F3)(0.2f, 0.3f) << endl;

    return 0;
}
