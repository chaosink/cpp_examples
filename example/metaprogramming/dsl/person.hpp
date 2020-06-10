#pragma once

#include <string>

#include "dsl.hpp"

namespace person {

struct record {
    std::string name;
    std::string surname;
    unsigned int age;
};

// Global object.
// `Member` is member variable.
auto name = field{&record::name};
auto surname = field{&record::surname};
auto age = field{&record::age};

} // namespace person
