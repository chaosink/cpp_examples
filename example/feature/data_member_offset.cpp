#include <iostream>
#include <cassert>
using namespace std;

struct A {
    int i = 7;
    char c = '8';
    float f = 9.f;
};

template<typename T, typename U>
/*constexpr*/ size_t OffsetOf(U T::*member) {
    return (char *)&((T *)nullptr->*member) - (char *)nullptr;
    // Non-constexpr.
    // No pointer difference or `reinterpret_cast` can be in a constant expression.
}

/*------------------------------------------------------------------------------------------------*/
// Inheritance.

struct B {
    double d = 10.;
};

struct C
    : public A
    , public B {};

/*------------------------------------------------------------------------------------------------*/
// Test.

int main() {
    /*--------------------------------------------------------------------------------------------*/
    // Output.

    printf("%d\n", &A::i); // 0
    printf("%d\n", &A::c); // 4
    printf("%d\n", &A::f); // 8

    cout << &A::i << endl; // 1
    cout << &A::c << endl; // 1
    cout << &A::f << endl; // 1

    /*--------------------------------------------------------------------------------------------*/
    // Offset of.

    static_assert(offsetof(A, i) == 0);
    static_assert(offsetof(A, c) == 4);
    static_assert(offsetof(A, f) == 8);

    assert(OffsetOf(&A::i) == 0);
    assert(OffsetOf(&A::c) == 4);
    assert(OffsetOf(&A::f) == 8);

    /*--------------------------------------------------------------------------------------------*/
    // Access.

    int A::*i_offset_A = &A::i;
    char A::*c_offset_A = &A::c;
    float A::*f_offset_A = &A::f;

    A a;

    assert(a.*i_offset_A == 7);
    assert(a.*c_offset_A == '8');
    assert(a.*f_offset_A == 9.f);

    assert(a.*&A::i == 7);
    assert(a.*&A::c == '8');
    assert(a.*&A::f == 9.f);

    assert(a.A::i == 7);
    assert(a.A::c == '8');
    assert(a.A::f == 9.f);

    /*--------------------------------------------------------------------------------------------*/
    // nullptr.

    double A::*offset_null = nullptr;
    printf("%d\n", offset_null); // -1

    /*--------------------------------------------------------------------------------------------*/
    // Downcast.

    double B::*d_offset_B = &B::d;
    double C::*d_offset_C = d_offset_B;
    printf("%d\n", d_offset_B); // 0
    printf("%d\n", d_offset_C); // 16

    return 0;
}
