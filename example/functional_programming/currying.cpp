#include <tuple>
#include <functional>
#include <type_traits>
#include <cassert>

template<typename Function, typename... CapturedArgs>
class curried {
    Function m_function;
    std::tuple<CapturedArgs...> m_captured;

public:
    curried(Function function, CapturedArgs... args)
        : m_function(function), m_captured(std::make_tuple(args...)) {}

    curried(Function function, std::tuple<CapturedArgs...> args)
        : m_function(function), m_captured(args) {}

    template<typename... NewArgs>
    auto operator()(NewArgs &&... args) const {
        auto new_args = std::make_tuple(std::forward<NewArgs>(args)...);
        auto all_args = std::tuple_cat(m_captured, std::move(new_args));

        if constexpr(std::is_invocable_v<Function, CapturedArgs..., NewArgs...>) {
            return std::apply(m_function, all_args);

        } else {
            return curried<Function, CapturedArgs..., NewArgs...>(m_function, all_args);
        }
    }
};

// Needed for pre-C++17 compilers
template<typename Function>
curried<Function> make_curried(Function &&f) {
    return curried<Function>(std::forward<Function>(f));
}

class callable_test {
public:
    template<typename T1, typename T2, typename T3>
    auto operator()(T1 x, T2 y, T3 z) const {
        return x + y + z;
    }

    template<typename T1, typename T2>
    auto operator()(T1 x, T2 y) const {
        return x + y;
    }
};

template<typename T>
class error;

int main() {
    auto less_curried = curried(std::less<>());
    assert(less_curried(42, 1) == false);

    auto greater_than_42 = less_curried(42);
    assert(greater_than_42(1.0) == false);
    assert(greater_than_42(100.0) == true);



    callable_test ct;

    auto ct_curried = curried(ct);
    assert(ct_curried(1)(2, 3) == 6);

    // `ct_curried_one(2)` returns an `int` which is not callable on `3`.
    // assert(ct_curried_one(2)(3) == 6); // Compilation error.

    auto ct_curried_one = curried(ct, 1);
    assert(ct_curried_one(2, 3) == 6);

    return 0;
}
