#include <iostream>
#include <cassert>
using namespace std;

template<unsigned long N>
struct binary {
    static unsigned const value = binary<N / 10>::value * 2 + N % 10;
};

template<> // specialization
struct binary<0> { // terminate recursion
    static unsigned const value = 0;
};

int main() {
    unsigned const one = binary<1>::value;
    unsigned const three = binary<11>::value;
    unsigned const five = binary<101>::value;
    unsigned const seven = binary<111>::value;
    unsigned const nine = binary<1001>::value;

    static_assert(one == 1);
    static_assert(three == 3);
    static_assert(five == 5);
    static_assert(seven == 7);
    static_assert(nine == 9);

    static_assert(binary<101010>::value == 42);
    // Even though 678 is not a valid binary value representation.
    static_assert(binary<678>::value == 46);
}
