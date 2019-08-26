#include <cassert>
using namespace std;

// Empty classes occupy 1 byte to get different addresses.
class B0 { // 1
};

// Empty classes may have virtual table ptr.
class B1 { // 8
    [[maybe_unused]] virtual void Fun() {}
};

class B2 { // 4
    [[maybe_unused]] int i;
};

// Memory alignment.
class B3 { // 8 = 4+1+(3)
    [[maybe_unused]] int i;
    [[maybe_unused]] char c;
};

// Memory alignment.
#pragma pack(2)
class B4 { // 6 = 4+1+(1)
    [[maybe_unused]] int i;
    [[maybe_unused]] char c;
};
#pragma pack()

// Static data members are per-class instead of per-object.
class B5 { // 4
    [[maybe_unused]] int i;
    static char c;
};

// Memory alignment and virtual table ptr.
class B6 { // 16 = 4+(4)+8
    [[maybe_unused]] int i;

    [[maybe_unused]] virtual void Fun0() {}
};

// Virtual table is per-class instead of per-object. Objects only have the ptr.
class B7 { // 16 = 4+(4)+8
    [[maybe_unused]] int i;

    [[maybe_unused]] virtual void Fun0() {}
    [[maybe_unused]] virtual void Fun1() {}
};

class D0 : B6 { // 16
    [[maybe_unused]] virtual void Fun() {}
};

class D1 : B6 { // 16
    [[maybe_unused]] virtual void Fun() {}
};

// D0: B6::vfptr, B6::i.
// D1: B6::vfptr, B6::i.
class F01 : D0, D1 { // 32 = 16+16
};

// vfptr, B6::vfptr, B6::i.
class D2 : virtual B6 { // 24 = 8+16
    [[maybe_unused]] virtual void Fun() {}
};

// vfptr, B6::vfptr, B6::i.
class D3 : virtual B6 { // 24 = 8+16
    [[maybe_unused]] virtual void Fun() {}
};

// D2: vfptr.
// D3: vfptr.
// B6: vfptr, i.
class F23 : D2, D3 { // 32 = 8+8+16
    [[maybe_unused]] virtual void Fun() {}
};

int main() {
    static_assert(sizeof(B0) == 1);
    static_assert(sizeof(B1) == 8);
    static_assert(sizeof(B2) == 4);
    static_assert(sizeof(B3) == 8);
    static_assert(sizeof(B4) == 6);
    static_assert(sizeof(B5) == 4);
    static_assert(sizeof(B6) == 16);
    static_assert(sizeof(B7) == 16);

    static_assert(sizeof(D0) == 16);
    static_assert(sizeof(D1) == 16);
    static_assert(sizeof(F01) == 32);

    static_assert(sizeof(D2) == 24);
    static_assert(sizeof(D3) == 24);
    static_assert(sizeof(F23) == 32);
}

/* clang++ -cc1 -fdump-record-layouts class_size.cpp

*** Dumping AST Record Layout
         0 | class B0 (empty)
           | [sizeof=1, dsize=1, align=1,
           |  nvsize=1, nvalign=1]

*** Dumping AST Record Layout
         0 | class B1
         0 |   (B1 vtable pointer)
           | [sizeof=8, dsize=8, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class B2
         0 |   int i
           | [sizeof=4, dsize=4, align=4,
           |  nvsize=4, nvalign=4]

*** Dumping AST Record Layout
         0 | class B3
         0 |   int i
         4 |   char c
           | [sizeof=8, dsize=5, align=4,
           |  nvsize=5, nvalign=4]

*** Dumping AST Record Layout
         0 | class B4
         0 |   int i
         4 |   char c
           | [sizeof=6, dsize=5, align=2,
           |  nvsize=5, nvalign=2]

*** Dumping AST Record Layout
         0 | class B5
         0 |   int i
           | [sizeof=4, dsize=4, align=4,
           |  nvsize=4, nvalign=4]

*** Dumping AST Record Layout
         0 | class B6
         0 |   (B6 vtable pointer)
         8 |   int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class B7
         0 |   (B7 vtable pointer)
         8 |   int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class D0
         0 |   class B6 (primary base)
         0 |     (B6 vtable pointer)
         8 |     int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class D1
         0 |   class B6 (primary base)
         0 |     (B6 vtable pointer)
         8 |     int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class F01
         0 |   class D0 (primary base)
         0 |     class B6 (primary base)
         0 |       (B6 vtable pointer)
         8 |       int i
        16 |   class D1 (base)
        16 |     class B6 (primary base)
        16 |       (B6 vtable pointer)
        24 |       int i
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=28, nvalign=8]

*** Dumping AST Record Layout
         0 | class D2
         0 |   (D2 vtable pointer)
         8 |   class B6 (virtual base)
         8 |     (B6 vtable pointer)
        16 |     int i
           | [sizeof=24, dsize=20, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class D3
         0 |   (D3 vtable pointer)
         8 |   class B6 (virtual base)
         8 |     (B6 vtable pointer)
        16 |     int i
           | [sizeof=24, dsize=20, align=8,
           |  nvsize=8, nvalign=8]

*** Dumping AST Record Layout
         0 | class F23
         0 |   class D2 (primary base)
         0 |     (D2 vtable pointer)
         8 |   class D3 (base)
         8 |     (D3 vtable pointer)
        16 |   class B6 (virtual base)
        16 |     (B6 vtable pointer)
        24 |     int i
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=16, nvalign=8]

*/
