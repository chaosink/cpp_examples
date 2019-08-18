#include <iostream>
#include <vector>
using namespace std;

class A {
    [[maybe_unused]] int i;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] A " << size << endl; // 40 = 4*10
        return malloc(size);
    }
};

class B {
    [[maybe_unused]] int i;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] B " << size << endl; // 40 = 4*10
        return malloc(size);
    }
    B() {}
};


class C {
    [[maybe_unused]] int i;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] C " << size << endl; // 48 = 4*10+8
        // The extra 8 bytes are for recording the count of objects.
        // Here it is required to know how many times dtor should be called, as dtor is non-trivial.
        return malloc(size);
    }
    ~C() {}
};

class D {
    [[maybe_unused]] int i;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] D " << size << endl; // 48 = 4*10+8
        // The extra 8 bytes are for recording the count of objects.
        // Here it is required by `operator delete[]()` which has parameter `size`.
        return malloc(size);
    }
    // `operator delete[]()` has parameter `size`.
    static void operator delete[](void *p, std::size_t size) {
        cout << "delete[] D " << size << endl; // 48 = 4*10+8
        free(p);
    }
};

class E {
    [[maybe_unused]] int i;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] E " << size << endl; // 40
        return malloc(size);
    }
    // `operator delete[]()` doesn't have parameter `size`, hence the count of objects is not required.
    static void operator delete[](void *p) {
        free(p);
    }
};

class F {
    [[maybe_unused]] std::vector<int> v;

public:
    static void *operator new[](std::size_t size) {
        cout << "new[] F " << size << endl; // 248 = 24*10+8
        // The extra 8 bytes are for recording the count of objects.
        // Here it is required to know how many times dtor should be called, as dtor is non-trivial
        // which should call the non-trivial dtor of data member `v`.
        return malloc(size);
    }
};

int main() {
    A *a = new A[10];
    B *b = new B[10];
    C *c = new C[10];
    D *d = new D[10];
    E *e = new E[10];
    F *f = new F[10];

    delete[] a;
    delete[] b;
    delete[] c;
    delete[] d;
    delete[] e;
    delete[] f;

    cout << "sizeof(A) " << sizeof(A) << endl; // 4
    cout << "sizeof(B) " << sizeof(B) << endl; // 4
    cout << "sizeof(C) " << sizeof(C) << endl; // 4
    cout << "sizeof(D) " << sizeof(D) << endl; // 4
    cout << "sizeof(E) " << sizeof(E) << endl; // 4
    cout << "sizeof(F) " << sizeof(F) << endl; // 24

    return 0;
}

/* output:
new[] A 40
new[] B 40
new[] C 48
new[] D 48
new[] E 40
new[] F 248
delete[] D 48
sizeof(A) 4
sizeof(B) 4
sizeof(C) 4
sizeof(D) 4
sizeof(E) 4
sizeof(F) 24
*/
