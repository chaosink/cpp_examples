#include <iostream>
#include <string>
#include <cassert>

#include "dsl.hpp"
#include "person.hpp"
#include "vehicle.hpp"

int main() {
    {
        using namespace person;

        record martha;

        // clang-format off
        with(martha)(
            name = "Martha",
            surname = "Jones"
        );
        // clang-format on
        assert(martha.name == "Martha");
    }

    {
        using namespace vehicle;

        record car;

        // clang-format off
        with(car)(
            model = "Ford T",
            color = "blue"
        );
        // clang-format on
        assert(car.model() == "Ford T");
        assert(car.color() == "blue");

        // clang-format off
        with(car)(
            model = "Ford S",
            color = "" // Invalid assignment. String cannot be empty.
                       // The whole transaction is refused.
        );
        // clang-format on
        assert(car.model() == "Ford T");
        assert(car.color() == "blue");
    }

    return 0;
}
