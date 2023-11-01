#include <iostream>
#include <iomanip>
#include <bit>
#include <cassert>
using namespace std;

#include "fp16.h"
#include "half.hpp"

typedef unsigned int uint;
typedef unsigned short ushort;

uint as_uint(const float x) {
    return *(uint *)&x;
}
float as_float(const uint x) {
    return *(float *)&x;
}

// clang-format off
//********************************************************************************
// https://stackoverflow.com/a/60047308/4306274
namespace so {
float half_to_float(const ushort x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
    const uint e = (x & 0x7C00) >> 10; // exponent
    const uint m = (x & 0x03FF) << 13; // mantissa
    const uint v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
    return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
}
ushort float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
    const uint b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
    const uint e = (b & 0x7F800000) >> 23; // exponent
    const uint m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF; // sign : normalized : denormalized : saturate
}
}
//********************************************************************************
// clang-format on

int main() {
    float a = 0.f;
    assert((1.f / a) == numeric_limits<float>::infinity());

    uint float32_i = 0x47ca9334;
    float float32_f = as_float(float32_i); // Should use `bit_cast<float>()` if C++20 is used.
    cout << fixed << setprecision(2) << float32_f << endl; // 103718.41

    cout << endl;

    // This implementation doesn't confirm to the IEEE-754 standard.
    ushort float16_i = so::float_to_half(float32_f);
    cout << "0x" << setfill('0') << setw(4) << right << hex << float16_i << endl; // 0x7e55
    float float16_i_to_float32_f = so::half_to_float(float16_i);
    cout << float16_i_to_float32_f << endl; // 103744.00
    cout << "0x" << setfill('0') << setw(8) << right << hex << as_uint(float16_i_to_float32_f)
         << endl; // 0x47caa000

    cout << endl;

    float16_i = fp16_ieee_from_fp32_value(float32_f);
    cout << "0x" << setfill('0') << setw(4) << right << hex << float16_i << endl; // 0x7c00
    float16_i_to_float32_f = fp16_ieee_to_fp32_value(float16_i);
    cout << float16_i_to_float32_f << endl; // inf
    cout << "0x" << setfill('0') << setw(8) << right << hex << as_uint(float16_i_to_float32_f)
         << endl; // 0x7f800000

    cout << endl;

    half_float::half float16_half(float32_f);
    cout << sizeof(float16_half) << endl; // 2
    cout << "0x" << setfill('0') << setw(4) << right << hex
         << *reinterpret_cast<ushort *>(&float16_half) << endl; // 0x7c00
    float16_i_to_float32_f = float(float16_half);
    cout << float16_i_to_float32_f << endl; // inf
    cout << "0x" << setfill('0') << setw(8) << right << hex << as_uint(float16_i_to_float32_f)
         << endl; // 0x7f800000
}

/*
# float32
103718.41

# Stack Overflow: This implementation doesn't confirm to the IEEE-754 standard.
0x7e55
103744
0x47caa000

# GT: fp16.h
0x7c00
inf
0x7f800000

# GT: half.hpp
0x7c00
inf
0x7f800000

# GT: OpenEXR half.h, not tested here.
0x7c00
inf
0x7f800000
*/
