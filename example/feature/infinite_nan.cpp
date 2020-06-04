#include <cmath>
#include <cassert>
#include <iostream>
#include <limits>
using namespace std;

using uint = unsigned;
constexpr float PI = 3.14159265358979323846;
constexpr float PI_2 = PI * 0.5;

int main() {
    constexpr float inf = numeric_limits<float>::infinity();
    constexpr float nan = numeric_limits<float>::quiet_NaN();

    cout << boolalpha;

    cout << "inf       = " << inf << endl;       // inf
    cout << "nan       = " << nan << endl;       // nan
    cout << "inf * inf = " << inf * inf << endl; // inf
    cout << "0 / inf   = " << 0 / inf << endl;   // 0
    cout << "1 / inf   = " << 1 / inf << endl;   // 0
    cout << "0 * inf   = " << 0 * inf << endl;   // nan, 0 for -ffast-math

    cout << endl;

    cout << "sin(inf)   = " << sin(inf) << endl;   // -nan
    cout << "sin(-inf)  = " << sin(-inf) << endl;  // -nan
    cout << "cos(inf)   = " << cos(inf) << endl;   // -nan
    cout << "cos(-inf)  = " << cos(-inf) << endl;  // -nan
    cout << "tan(inf)   = " << tan(inf) << endl;   // -nan
    cout << "tan(-inf)  = " << tan(-inf) << endl;  // -nan
    cout << "asin(2)    = " << asin(2) << endl;    // nan
    cout << "acos(2)    = " << acos(2) << endl;    // nan
    cout << "tgamma(-1) = " << tgamma(-1) << endl; // nan, -nan for -ffast-math
    cout << "log(-1)    = " << log(-1) << endl;    // nan
    cout << "sqrt(-1)   = " << sqrt(-1) << endl;   // -nan
#ifndef _MSC_BUILD
    cout << "0 / 0.f    = " << 0 / 0.f << endl;    // -nan, nan for -ffast-math
#endif

    cout << endl;

    cout << "NaN == NaN    ? " << (nan == nan) << endl;  // false, true for -ffast-math
    cout << "isfinite(NaN) = " << isfinite(nan) << endl; // false
    cout << "nan == 0.f    ? " << (nan == 0.f) << endl;  // false
    cout << "(double)nan   = " << (double)nan << endl;   // nan

    cout << endl;

    float f1 = 1.f;
    printf("1.f -> %x\n", *(uint*)&f1);  // 3f800000
    printf("inf -> %x\n", *(uint*)&inf); // 7f800000
    printf("nan -> %x\n", *(uint*)&nan); // 7fc00000

    cout << endl;

    uint i = 0xffc00000;
    float g = *(float*)&i;
    cout << "g             = " << g << endl;             // -nan
    cout << "g == 0.f      ? " << (g == 0.f) << endl;    // false
    cout << "g != 0.f      ? " << (g != 0.f) << endl;    // true
    cout << "isnan(g)      = " << isnan(g) << endl;      // true, false for -ffast-math
    cout << "isfinite(g)   = " << isfinite(g) << endl;   // false
    cout << "isfinite(1.f) = " << isfinite(1.f) << endl; // true

    float atan_inf = atan(inf);
    assert(atan_inf == PI_2);
    float atan_neg_inf = atan(-inf);
    assert(atan_neg_inf == -PI_2);
    float tan_pi_2 = tan(PI_2);
    assert(!isinf(tan_pi_2));
}
