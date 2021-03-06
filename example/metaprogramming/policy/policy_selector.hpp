#pragma once

#include "traits.hpp"
#include "policy_container.hpp"

namespace policy {
namespace policy_selector {

template<typename TPolicyCont>
struct PolicySelRes;

template<typename TCurPolicy, typename... TOtherPolicies>
struct PolicySelRes<PolicyContainer<TCurPolicy, TOtherPolicies...>>
    : TCurPolicy
    , TOtherPolicies... {};

/// ====================== Split Major Class ============================
template<typename MCO, typename TMajorClass, typename... TP>
struct MajorFilter_ {
    using type = MCO;
};

template<typename... TFilteredPolicies, typename TMajorClass, typename TCurPolicy, typename... TP>
struct MajorFilter_<PolicyContainer<TFilteredPolicies...>, TMajorClass, TCurPolicy, TP...> {
    template<typename CurMajor, typename TDummy = void>
    struct _impl {
        using type =
            typename MajorFilter_<PolicyContainer<TFilteredPolicies...>, TMajorClass, TP...>::type;
    };

    template<typename TDummy>
    struct _impl<TMajorClass, TDummy> {
        using type = typename MajorFilter_<PolicyContainer<TFilteredPolicies..., TCurPolicy>,
                                           TMajorClass,
                                           TP...>::type;
    };
    using type = typename _impl<typename TCurPolicy::MajorClass>::type;
};

/// ================= Minor Check ===============================
template<typename TMinorClass, typename... TP>
struct MinorDedup_ {
    static constexpr bool value = true;
};

template<typename TMinorClass, typename TCurPolicy, typename... TP>
struct MinorDedup_<TMinorClass, TCurPolicy, TP...> {
    using TCurMinor = typename TCurPolicy::MinorClass;
    constexpr static bool cur_check = !(std::is_same<TMinorClass, TCurMinor>::value);
    constexpr static bool value = AndValue<cur_check, MinorDedup_<TMinorClass, TP...>>;
};

template<typename TPolicyCont>
struct MinorCheck_ {
    static constexpr bool value = true;
};

template<typename TCurPolicy, typename... TP>
struct MinorCheck_<PolicyContainer<TCurPolicy, TP...>> {
    static constexpr bool cur_check = MinorDedup_<typename TCurPolicy::MinorClass, TP...>::value;

    static constexpr bool value = AndValue<cur_check, MinorCheck_<PolicyContainer<TP...>>>;
};

template<typename TMajorClass, typename TPolicyContainer>
struct Selector_;

template<typename TMajorClass, typename... TPolicies>
struct Selector_<TMajorClass, PolicyContainer<TPolicies...>> {
    using TMF = typename MajorFilter_<PolicyContainer<>, TMajorClass, TPolicies...>::type;
    static_assert(MinorCheck_<TMF>::value, "Minor class set conflict!");

    using type = std::conditional_t<IsArrayEmpty<TMF>, TMajorClass, PolicySelRes<TMF>>;
};

} // namespace policy_selector

template<typename TMajorClass, typename TPolicyContainer>
using PolicySelect = typename policy_selector::Selector_<TMajorClass, TPolicyContainer>::type;

} // namespace policy
