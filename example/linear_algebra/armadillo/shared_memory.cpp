#include <iostream>
#include <cassert>
using namespace std;

#include <armadillo>
using namespace arma;

#define Print(x) cout << #x << " =" << endl << x << endl;

int main() {
    arma::Mat<float> a = {{1, 2}, {3, 4}};

    arma::Mat<float> b(a.memptr(), 1, 4);
    assert(b.memptr() != a.memptr()); // Memory is allocated and copied.

    // `false` means no memory allocation and copying, hence memory is shared.
    arma::Mat<float> c(a.memptr(), 1, 4, false);
    assert(c.memptr() == a.memptr());

    c[0] = 10;
    assert(c.memptr() == a.memptr());
    Print(a);
    Print(c);

    c = {1, 2, 3, 4, 5, 6, 7, 8}; // Memory is allocated and copied.
    assert(c.memptr() != a.memptr());
    Print(a);
    Print(c);

    return 0;
}
