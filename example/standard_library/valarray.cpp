#include <cassert>
#include <iostream>
#include <valarray>
using namespace std;

int main() {
    const std::valarray<float> v1{1.f};
    const std::valarray<float> v2{10.f, 20.f};
    const std::valarray<float> v3{100.f, 200.f, 300.f};
    assert(v1.size() == 1);
    assert(v2.size() == 2);
    assert(v3.size() == 3);

    // Operations on valarrays with different sizes are undefined behaviours.
    std::valarray<float> v1_plus_v2 = v1 + v2;
    std::valarray<float> v2_plus_v1 = v2 + v1;
    assert(v1_plus_v2.size() == 1); // `v1` is at the first. Hence 2 elements are processed.
    assert(v2_plus_v1.size() == 2); // `v2` is at the first. Hence only 1 element is processed.


    { // Copy assignment.
        std::valarray<float> v2_assign_v1 = v2;
        v2_assign_v1 = v1;
        assert(v2_assign_v1.size() == 1);
        assert(v2_assign_v1[0] == 1.f);

        std::valarray<float> v2_assign_v3 = v2;
        v2_assign_v3 = v3;
        assert(v2_assign_v3.size() == 3);
        assert(v2_assign_v3[0] == 100.f);
        assert(v2_assign_v3[1] == 200.f);
        assert(v2_assign_v3[2] == 300.f);
    }

    { // Use `std::slice` to preserve the size.
        std::valarray<float> v2_assign_v1 = v2;
        v2_assign_v1[slice(0, v2_assign_v1.size(), 1)] = v1;
        assert(v2_assign_v1.size() == 2);
        assert(v2_assign_v1[0] == 1.f);
        assert(v2_assign_v1[1] == 0.f);

        std::valarray<float> v2_assign_v3 = v2;
        v2_assign_v3[slice(0, v2_assign_v3.size(), 1)] = v3;
        assert(v2_assign_v3.size() == 2);
        assert(v2_assign_v3[0] == 100.f);
        assert(v2_assign_v3[1] == 200.f);
    }

    return 0;
}
