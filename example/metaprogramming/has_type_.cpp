#include <type_traits>
using namespace std;

template<typename... T>
using Void = void;

#define HAS_TYPE(NAME)                           \
    template<typename, typename = void>          \
    struct has_type_##NAME : std::false_type {}; \
    template<typename T>                         \
    struct has_type_##NAME<T, Void<typename T::NAME>> : std::true_type {};

HAS_TYPE(bar)



#define HAS_TYPE_(NAME)                                \
    template<typename T>                               \
    std::true_type _has_type_##NAME(typename T::foo*); \
    template<typename T>                               \
    std::false_type _has_type_##NAME(...);             \
    template<typename T>                               \
    using has_type_##NAME = decltype(_has_type_##NAME<T>(nullptr));

HAS_TYPE_(foo)



struct A {
    using bar = int;
};
struct B {
    using foo = float;
};

int main() {
    static_assert(has_type_bar<A>::value == true);
    static_assert(has_type_bar<B>::value == false);

    static_assert(has_type_foo<A>::value == false);
    static_assert(has_type_foo<B>::value == true);
}
