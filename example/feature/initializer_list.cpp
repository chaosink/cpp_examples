#include <valarray>
#include <vector>
#include <cassert>
using namespace std;

int main() {
    {
        vector<int> a(2);
        vector<int> b{2};
        assert(a.size() == 2);
        assert(a[0] == 0);
        assert(b.size() == 1);
        assert(b[0] == 2);
    }

    {
        valarray<int> a(2);
        valarray<int> b{2};
        assert(a.size() == 2);
        assert(a[0] == 0);
        assert(b.size() == 1);
        assert(b[0] == 2);
    }

    return 0;
}
