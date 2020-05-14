#include <iostream>
#include <map>
#include <mutex>
#include <tuple>
#include <type_traits>

namespace detail {

class null_param {};

template<class Sig, class F>
class memoize_helper;

template<class Result, class... Args, class F>
class memoize_helper<Result(Args...), F> {
private:
    using function_type = F;
    using args_tuple_type = std::tuple<std::decay_t<Args>...>;

    function_type f;
    // Defines the cache, and, because it’s  mutable, synchronizes
    // all the changes
    mutable std::map<args_tuple_type, Result> m_cache;
    mutable std::recursive_mutex m_cache_mutex;

public:
    // The constructors need to initialize the wrapped function.
    // You could made copy-constructor copy the cached values as well,
    // but that’s not necessary.
    template<typename Function>
    memoize_helper(Function &&f, null_param) : f(f) {}

    memoize_helper(const memoize_helper &other) : f(other.f) {}

    template<class... InnerArgs>
    Result operator()(InnerArgs &&... args) const {
        std::unique_lock<std::recursive_mutex> lock{m_cache_mutex};

        // Searches for the cached value
        const auto args_tuple = std::make_tuple(args...);
        const auto cached = m_cache.find(args_tuple);

        //  If the cached value is found, returns it without calling f
        if(cached != m_cache.end()) {
            return cached->second;
        } else {
            // If the cached value isn’t found, calls f and stores
            // the result. Passes *this as the first argument: the
            // function to be used for the recursive call.
            auto &&result = f(*this, std::forward<InnerArgs>(args)...);
            m_cache[args_tuple] = result;
            return result;
        }
    }
};

} // namespace detail

using detail::memoize_helper;

// Major Premise: Sig = Result(Args...)
// Minor Premise: Sig = unsigned int(unsigned int)
// Conclusion   : Result = unsigned int, Args... = unsigned int
template<class Sig, class F>
memoize_helper<Sig, std::decay_t<F>> make_memoized_r(F &&f) {
    return {std::forward<F>(f), detail::null_param{}};
}

int main() {
    // Sig = unsigned int(unsigned int), F = type of lambda
    auto fibmemo = make_memoized_r<unsigned int(unsigned int)>([](auto &fib, unsigned int n) {
        std::cout << "Calculating " << n << "!\n";
        return n == 0 ? 0 : n == 1 ? 1 : fib(n - 1) + fib(n - 2);
    });

    std::cout << "15! = \n" << fibmemo(15) << std::endl; // Evaluate for the first time.
    std::cout << "15! = \n" << fibmemo(15) << std::endl; // Use memoization.

    return 0;
}
