#include <ratio>

#include <cassert>

template<typename Representation, typename Ratio = std::ratio<1>>
class distance {
    Representation value;

public:
    explicit constexpr distance(Representation value): value(value) {}

    template<typename R>
    auto operator+(const distance<Representation, R> &other) const {
        return distance<Representation, std::ratio<1>>(meter() + other.meter());
    }

    Representation meter() const {
        return value * Ratio::num / Ratio::den;
    }
};



template<typename Representation>
using meters = distance<Representation>;

template<typename Representation>
using kilometers = distance<Representation, std::kilo>;

template<typename Representation>
using centimeters = distance<Representation, std::centi>;

template<typename Representation>
using miles = distance<Representation, std::ratio<1609>>;



namespace distance_literals {

constexpr kilometers<long double> operator""_km(long double distance) {
    return kilometers<long double>(distance);
}

constexpr miles<long double> operator""_mi(long double distance) {
    return miles<long double>(distance);
}

} // namespace distance_literals



int main() {
    using namespace distance_literals;

    auto distance_in_kilometers = 1.0_km;
    auto distance_in_miles = 1.0_mi;

    auto more_distance = distance_in_kilometers + distance_in_kilometers;
    assert(more_distance.meter() == 2000);

    auto total_distance = distance_in_kilometers + distance_in_miles;
    assert(total_distance.meter() == 2609);

    return 0;
}
