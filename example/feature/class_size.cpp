#include <cassert>
using namespace std;

// Empty classes occupy 1 byte to get different addresses.
class B0 { // 1
};

// Empty classes may have virtual table ptr.
class B1 { // 8
    virtual void Fun() {}
};

class B2 { // 4
    int i;
};

// Memory alignment.
class B3 { // 8 = 4+1+(3)
    int i;
    char c;
};

// Memory alignment.
#pragma pack(2)
class B4 { // 6 = 4+1+(1)
    int i;
    char c;
};
#pragma pack()

// Static data members are per-class instead of per-object.
class B5 { // 4
    int i;
    static char c;
};

// Memory alignment and virtual table ptr.
class B6 { // 16 = 4+(4)+8
    int i;

    virtual void Fun0() {}
};

// Virtual table is per-class instead of per-object. Objects only have the ptr.
class B7 { // 16 = 4+(4)+8
    int i;

    virtual void Fun0() {}
    virtual void Fun1() {}
};

class D0: B6 { // 16
    virtual void Fun() {}
};

class D1: B6 { // 16
    virtual void Fun() {}
};

// clang-format off
// D0: B6::vptr, B6::i.
// D1: B6::vptr, B6::i. // Two `B6::i`.
class F01 : D0, D1 { // 32 = 16+16
};
// clang-format on

// vptr, B6::vptr, B6::i.
class D2: virtual B6 { // 24 = 8+16
    virtual void Fun() {}
};

// vptr, B6::vptr, B6::i.
class D3: virtual B6 { // 24 = 8+16
    virtual void Fun() {}
};

// clang-format off
// D2: vptr.
// D3: vptr.
// B6: vptr, i. // Only one `B6::i`.
class F23 : D2, D3 { // 32 = 8+8+16
    virtual void Fun() {}
};
// clang-format on



class APrivate {
private:
    int i;
    char c;
};

class BPrivate: public APrivate {
private:
    char c;
};

class CPrivate: public BPrivate {
private:
    char c;
};

class APublic {
public:
    int i;
    char c;
};

class BPublic: public APublic {
public:
    char c;
};

class CPublic: public BPublic {
public:
    char c;
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



    static_assert(sizeof(APrivate) == 8);
    static_assert(sizeof(BPrivate) == 8);
    static_assert(sizeof(CPrivate) == 8);

    static_assert(sizeof(APublic) == 8);
    static_assert(sizeof(BPublic) == 12);
    static_assert(sizeof(CPublic) == 12);
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

*** Dumping AST Record Layout
         0 | class APrivate
         0 |   int i
         4 |   char c
           | [sizeof=8, dsize=5, align=4,
           |  nvsize=5, nvalign=4]

*** Dumping AST Record Layout
         0 | class BPrivate
         0 |   class APrivate (base)
         0 |     int i
         4 |     char c
         5 |   char c
           | [sizeof=8, dsize=6, align=4,
           |  nvsize=6, nvalign=4]

*** Dumping AST Record Layout
         0 | class CPrivate
         0 |   class BPrivate (base)
         0 |     class APrivate (base)
         0 |       int i
         4 |       char c
         5 |     char c
         6 |   char c
           | [sizeof=8, dsize=7, align=4,
           |  nvsize=7, nvalign=4]

*** Dumping AST Record Layout
         0 | class APublic
         0 |   int i
         4 |   char c
           | [sizeof=8, dsize=8, align=4,
           |  nvsize=8, nvalign=4]

*** Dumping AST Record Layout
         0 | class BPublic
         0 |   class APublic (base)
         0 |     int i
         4 |     char c
         8 |   char c
           | [sizeof=12, dsize=9, align=4,
           |  nvsize=9, nvalign=4]

*** Dumping AST Record Layout
         0 | class CPublic
         0 |   class BPublic (base)
         0 |     class APublic (base)
         0 |       int i
         4 |       char c
         8 |     char c
         9 |   char c
           | [sizeof=12, dsize=10, align=4,
           |  nvsize=10, nvalign=4]

*/
