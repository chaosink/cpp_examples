#pragma once

namespace policy {

template<typename... TPolicies>
struct PolicyContainer;

template<typename T>
constexpr bool IsPolicyContainer = false;

template<typename... T>
constexpr bool IsPolicyContainer<PolicyContainer<T...>> = true;

} // namespace policy
