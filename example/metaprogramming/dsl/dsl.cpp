#include <iostream>
#include <string>
#include <cassert>

#include "dsl.h"
#include "person.h"
#include "vehicle.h"

int main() {
    {
        using namespace person;

        record martha;

        with(martha)(
            name = "Martha",
            surname = "Jones"
        );
        assert(martha.name == "Martha");
    }

    {
        using namespace vehicle;

        record car;

        with(car)(
            model = "Ford T",
            color = "blue"
        );
        assert(car.model() == "Ford T");
        assert(car.color() == "blue");

        with(car)(
            model = "Ford S",
            color = "" // Invalid assignment. String cannot be empty.
                       // The whole transaction is refused.
        );
        assert(car.model() == "Ford T");
        assert(car.color() == "blue");
    }

    return 0;
}
