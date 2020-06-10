#include "policy_container.hpp"
#include "policy_selector.hpp"
using namespace policy;

#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

struct AccPolicy {
    using MajorClass = AccPolicy;

    struct AccuTypeCate {
        struct Add;
        struct Mul;
    };
    using Accu = AccuTypeCate::Add;

    struct IsAveValueCate;
    static constexpr bool IsAve = false;

    struct ValueTypeCate;
    using Value = float;
};

// clang-format off
#include "policy_macro_begin.hpp"
TypePolicyObj      (PAddAccu,     AccPolicy, Accu,  Add);
TypePolicyObj      (PMulAccu,     AccPolicy, Accu,  Mul);
ValuePolicyObj     (PAve,         AccPolicy, IsAve, true);
ValuePolicyObj     (PNoAve,       AccPolicy, IsAve, false);
TypePolicyTemplate (PValueTypeIs, AccPolicy, Value);
ValuePolicyTemplate(PAvePolicyIs, AccPolicy, IsAve);
#include "policy_macro_end.hpp"
// clang-format on

template<typename... TPolicies>
struct Accumulator {
    using TPoliCont = PolicyContainer<TPolicies...>;
    using TPolicyRes = PolicySelect<AccPolicy, TPoliCont>;

    using ValueType = typename TPolicyRes::Value;
    static constexpr bool is_ave = TPolicyRes::IsAve;
    using AccuType = typename TPolicyRes::Accu;

public:
    template<typename TIn>
    static auto Eval(const TIn &in) {
        if constexpr(std::is_same<AccuType, AccPolicy::AccuTypeCate::Add>::value) {
            ValueType count = 0;
            ValueType res = 0;
            for(const auto &x: in) {
                res += x;
                count += 1;
            }

            if constexpr(is_ave)
                return res / count;
            else
                return res;
        } else if constexpr(std::is_same<AccuType, AccPolicy::AccuTypeCate::Mul>::value) {
            ValueType res = 1;
            ValueType count = 0;
            for(const auto &x: in) {
                res *= x;
                count += 1;
            }
            if constexpr(is_ave)
                return pow(res, 1.0 / count);
            else
                return res;
        } else {
            static_assert(DependencyFalse<AccuType>);
        }
    }
};

template<typename... TPolicies, typename TIn>
auto Accumulate(const TIn &in) {
    using TPoliCont = PolicyContainer<TPolicies...>;
    using TPolicyRes = PolicySelect<AccPolicy, TPoliCont>;

    using ValueType = typename TPolicyRes::Value;
    constexpr bool is_ave = TPolicyRes::IsAve;
    using AccuType = typename TPolicyRes::Accu;

    if constexpr(std::is_same<AccuType, AccPolicy::AccuTypeCate::Add>::value) {
        ValueType count = 0;
        ValueType res = 0;
        for(const auto &x: in) {
            res += x;
            count += 1;
        }

        if constexpr(is_ave)
            return res / count;
        else
            return res;
    } else if constexpr(std::is_same<AccuType, AccPolicy::AccuTypeCate::Mul>::value) {
        ValueType res = 1;
        ValueType count = 0;
        for(const auto &x: in) {
            res *= x;
            count += 1;
        }
        if constexpr(is_ave)
            return pow(res, 1.0 / count);
        else
            return res;
    } else {
        static_assert(DependencyFalse<AccuType>);
    }
}

void test_policy_selector() {
    const int a[] = {1, 2, 3, 4, 5};

    cout << "Test class template policy selector... ";
    assert(fabs(Accumulator<>::Eval(a) - 15) < 0.0001);
    assert(fabs(Accumulator<PMulAccu>::Eval(a) - 120) < 0.0001);
    assert(fabs(Accumulator<PMulAccu, PAve>::Eval(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulator<PAve, PMulAccu>::Eval(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulator<PAvePolicyIs<true>, PMulAccu>::Eval(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulator<PValueTypeIs<double>, PMulAccu, PAve>::Eval(a) - pow(120.0, 0.2))
           < 0.0001);
    assert(fabs(Accumulator<PValueTypeIs<int>, PMulAccu, PAve>::Eval(a) - pow(120.0, 0.2))
           < 0.0001);
    // Accumulator<PAddAccu, PMulAccu>::Eval(a); // Minor class set conflict!
    cout << "done" << endl;

    cout << "Test function template policy selector... ";
    assert(fabs(Accumulate<>(a) - 15) < 0.0001);
    assert(fabs(Accumulate<PMulAccu>(a) - 120) < 0.0001);
    assert(fabs(Accumulate<PMulAccu, PAve>(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulate<PAve, PMulAccu>(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulate<PAvePolicyIs<true>, PMulAccu>(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulate<PValueTypeIs<double>, PMulAccu, PAve>(a) - pow(120.0, 0.2)) < 0.0001);
    assert(fabs(Accumulate<PValueTypeIs<int>, PMulAccu, PAve>(a) - pow(120.0, 0.2)) < 0.0001);
    // Accumulate<PAddAccu, PMulAccu>(a); // Minor class set conflict!
    cout << "done" << endl;
}

int main() {
    test_policy_selector();
}
