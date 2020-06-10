#include <functional>
#include <iostream>
#include <mutex>
#include <optional>

template<typename F>
class LazyVal {
    F evaluate_;
    // Memoization of evaluation result.
    mutable std::optional<decltype(evaluate_())> value_;
    mutable std::mutex value_lock_;

public:
    LazyVal(F function): evaluate_(function) {}
    LazyVal(LazyVal &&other): evaluate_(std::move(other.evaluate_)) {}
    operator decltype(evaluate_())() const {
        std::lock_guard<std::mutex> lock(value_lock_);
        if(!value_)
            value_ = std::invoke(evaluate_);
        return *value_;
    }
};

struct {
    template<typename F>
    auto operator-(F &&function) const {
        return LazyVal<F>(function);
    }
} MakeLazyValHelper;
// `LAZY` keyword, with which we can write only lambda body.
#define LAZY MakeLazyValHelper - [=] // Captured by value.

int main() {
    {
        int number = 6;
        auto number_inc = LAZY { // `LAZY` keyword.
            std::cout << "Calculating..." << std::endl;
            std::cout << "`number` is: " << number << std::endl;
            return number + 1;
        };
        std::cout << "Lazy value defined" << std::endl;
        number = 8; // Doesn't affect the calculation result.
        int result = number_inc; // Evaluation.
        std::cout << "Inc result: " << result << std::endl;
    }

    std::cout << std::endl;

    {
        int number = 8;
        LazyVal number_dec = [=]() { // CTAD (Class Template Argument Deduction) since C++17
            std::cout << "Calculating..." << std::endl;
            std::cout << "`number` is: " << number << std::endl;
            return number - 1;
        };
        std::cout << "Lazy value defined" << std::endl;
        number = 6; // Doesn't affect the calculation result.
        int result = number_dec; // Evaluation.
        std::cout << "Dec result: " << result << std::endl;
    }
}
