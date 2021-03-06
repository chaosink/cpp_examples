#include <type_traits>
using namespace std;

using uint = unsigned;

template<uint... V>
struct UintArray {};

template<typename T0, typename T1>
struct UintArrayConcat;

template<uint... V0, uint... V1> // no correspondence to <typename T0, typename T1>
struct UintArrayConcat<UintArray<V0...>,
                       UintArray<V1...>> { // correspondence to <typename T0, typename T1>
    using type = UintArray<V0..., V1...>;
};

template<typename T, uint V>
struct UintArrayAppend;

template<uint... V0, uint V1> // no correspondence to <typename T, uint V>
struct UintArrayAppend<UintArray<V0...>, V1> { // correspondence to <typename T, uint V>
    using type = UintArray<V0..., V1>;
};

template<uint V, typename T>
struct UintArrayPrepend;

template<uint V0, uint... V1> // no correspondence to <uint V, typename T>
struct UintArrayPrepend<V0, UintArray<V1...>> { // correspondence to <uint V, typename T>
    using type = UintArray<V0, V1...>;
};

template<typename... T>
struct sizes_of {};

template<typename T>
struct sizes_of<T> {
    using type = UintArray<sizeof(T)>;
};

template<typename S, typename... T>
struct sizes_of<S, T...> {
    // using type = typename UintArrayConcat<UintArray<sizeof(S)>, typename sizes_of<T...>::type>::type;
    // using type = typename UintArrayConcat<typename sizes_of<S>::type, typename sizes_of<T...>::type>::type;
    using type = typename UintArrayPrepend<sizeof(S), typename sizes_of<T...>::type>::type;
};

int main() {
    // clang-format off
#ifndef _MSC_BUILD // Assertion faild for MSVC. Don't know the reason for the moment.
    static_assert(is_same<
            sizes_of<char, int, long, float, double>::type,
            UintArray<1, 4, 8, 4, 8>
        >::value == true);
#endif

    [[maybe_unused]] sizes_of<char, int, long, float, double>::type sizes;
    [[maybe_unused]] UintArray<1, 4, 8, 4, 8> result;

#ifndef _MSC_BUILD
    static_assert(is_same<
            decltype(sizes),
            decltype(result)
        >::value == true);
#endif
    // clang-format on
}
