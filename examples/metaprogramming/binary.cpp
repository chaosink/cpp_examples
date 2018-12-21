#include <iostream>
using namespace std;

template<unsigned long N>
struct binary {
	static unsigned const value =
		binary<N/10>::value * 2 + N % 10;
};

template<> // specialization
struct binary<0> { // terminate recursion
	static unsigned const value = 0;
};

int main() {
	unsigned const one   = binary<1>::value;
	unsigned const three = binary<11>::value;
	unsigned const five  = binary<101>::value;
	unsigned const seven = binary<111>::value;
	unsigned const nine  = binary<1001>::value;

	cout << one   << endl;
	cout << three << endl;
	cout << five  << endl;
	cout << seven << endl;
	cout << nine  << endl;

	cout << binary<101010>::value << endl; // 42
	cout << binary<678>::value << endl;    // 46, even though not a valid binary value
	// representation
}
