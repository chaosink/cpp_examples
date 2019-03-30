#include <iostream>
using namespace std;

template<bool cur, typename TNext>
constexpr static bool OrValue = true;
template<typename TNext>
constexpr static bool OrValue<false, TNext> = TNext::value;

template<int... V>
struct HasOne {
};
template<int V>
struct HasOne<V> {
	constexpr static bool value = V == 1;
};
template<int V0, int... V1>
struct HasOne<V0, V1...> {
	constexpr static bool value = OrValue<V0 == 1, HasOne<V1...>>;
};



template<int cur, typename TNext>
constexpr static int OneOrSum = cur + TNext::value;
template<typename TNext>
constexpr static int OneOrSum<1, TNext> = 1;

template<int... V>
struct SumTillOne {
};
template<int V>
struct SumTillOne<V> {
	constexpr static int value = V;
};
template<int V0, int... V1>
struct SumTillOne<V0, V1...> {
	constexpr static int value = OneOrSum<V0, SumTillOne<V1...>>;
};



int main() {
	cout << HasOne<3, 1, 2>::value << endl; // 1
	cout << HasOne<3, 2, 2>::value << endl; // 0

	cout << SumTillOne<3, 1, 2>::value << endl; // 4 = 3 + 1
	cout << SumTillOne<3, 2, 2>::value << endl; // 7 = 3 + 2 + 2
}
