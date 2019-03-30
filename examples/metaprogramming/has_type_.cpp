#include <iostream>
#include <type_traits>
using namespace std;

template<typename...>
struct void_type {
	using type = void;
};

template<typename ...T>
using void_t = typename void_type<T...>::type;

#define HAS_TYPE(NAME) \
template<typename, typename = void> \
struct has_type_##NAME: std::false_type {}; \
template<typename T> \
struct has_type_##NAME<T, void_t<typename T::NAME>>: std::true_type {}; \

HAS_TYPE(bar)

struct A {
	using bar = int;
};
struct B {
};

int main() {
	cout << has_type_bar<A>::value << endl; // 1
	cout << has_type_bar<B>::value << endl; // 0
}
