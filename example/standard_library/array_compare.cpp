#include <cassert>
#include <iostream>
#include <array>
using namespace std;

int main() {
    {
        int a[2]{1, 2};
        int b[2]{3, 4};
        int c[2]{1, 2};
        // Array addresses compared.
        assert(a < b);
        assert(c > b);
    }
    {
        std::array<int, 2> a{1, 2};
        std::array<int, 2> b{3, 4};
        std::array<int, 2> c{1, 2};
        // Array values compared.
        assert(a < b);
        assert(c < b);
    }
    return 0;
}
