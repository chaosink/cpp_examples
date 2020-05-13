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

struct A {
    using bar = int;
};
struct B {};

int main() {
    static_assert(has_type_bar<A>::value == true);
    static_assert(has_type_bar<B>::value == false);
}
