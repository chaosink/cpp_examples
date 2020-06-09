class B {
public:
    int i = 1;
    virtual void Foo() {}
    virtual void Bar() {}
};

class D0 : virtual public B {
public:
    int j0 = 2;
    virtual void Foo() {}
    virtual void Bar() {}
};

class D1 : virtual public B {
public:
    int j1 = 3;
    virtual void Foo() {}
    virtual void Bar() {}
};

class F : public D0, public D1 {
public:
    int k = 4;
    virtual void Foo() {}
    virtual void Bar() {}
};

void Assign(B *b) {
    // mov dword ptr [rdi + 8], 10
    b->i = 10;

    // mov rax, qword ptr [rdi]
    // jmp qword ptr [rax + 8] # TAILCALL
    b->Bar();
    // `Foo()` is at [rax + 0];
    // `Bar()` is at [rax + 8].
}

/*
*** Dumping AST Record Layout
         0 | class F
         0 |   class D0 (primary base) ----------+
         0 |     (D0 vtable pointer)   // <- rbx |
         8 |     int j0                          |
        16 |   class D1 (base) --------+         +---------------------------------------------+---+
        16 |     (D1 vtable pointer)   |         |                                             |   |
        24 |     int j1                +---------------------------------------------------------+ |
        28 |   int k                   |         |                                             | | |
        32 |   class B (virtual base) -+---------+                                             | | |
        32 |     (B vtable pointer)    // <- rbx + ptr [ptr [rbx] - 24], i.e. rbx + 32         | | |
        40 |     int i                                                                         | | |
           | [sizeof=48, dsize=44, align=8,                                                    | | |
           |  nvsize=32, nvalign=8]                                                            | | |
                                                                                               | | |
vtable for F:                                                                                  | | |
                       // There may be more offsets for other virtual base classes.            | | |
                       // See: https://godbolt.org/z/kLtdFD                                    | | |
  .quad 32             // <- ptr [rbx] - 24, offset to virtual base class `B` in the layout. --+ | |
  .quad 0                                                                                        | |
  .quad typeinfo for F                                                                           | |
  .quad F::Foo()       // <- ptr [rbx], D0 vtable pointer                                        | |
  .quad F::Bar()                                                                                 | |
  .quad 16             // offset to virtual base class `B` in the layout ------------------------+ |
  .quad -16            // offset to top                                                            |
  .quad typeinfo for F                                                                             |
  .quad non-virtual thunk to F::Foo() // <- D1 vtable pointer                                      |
  .quad non-virtual thunk to F::Bar() // There may be more non-virtual blocks in the layout.       |
                                      // See: https://godbolt.org/z/k3lg_6                         |
                                      // Thunks in different non-virtual blocks with the same      |
                                      // names have different addresses.                           |
  .quad -32            // offset of `this` for `virtual thunk to F::Bar()` ------------------------+
  .quad -32            // offset of `this` for `virtual thunk to F::Foo()` ------------------------+
  .quad -32            // offset to top
  .quad typeinfo for F
  .quad virtual thunk to F::Foo() // <- B vtable pointer
  .quad virtual thunk to F::Bar() // There may be more virtual blocks in the layout.
                                  // See: https://godbolt.org/z/kLtdFD
                                  // Thunks in different virtual blocks with the same names have
                                  // the same addresses.

// About thunks:
// Thunks are for changing the memory layout pointer (rdi) from pointing to the base class subobject
// areas to pointing to the beginning of the whole memory layout.
// A thunk is virtual if the corresponding base class is virtual.

*/

int main() {
    static_assert(sizeof(F) == 48);
    F *f = new F();
    // mov rax, qword ptr [rbx]
    // mov rcx, qword ptr [rax - 24]
    // mov dword ptr [rbx + rcx + 8], 10 // rcx==32 is a runtime value, `8` is a compile-time value.
    f->i = 10;
    // mov rdi, qword ptr [rax - 24]
    // add rdi, rbx
    // call Assign(B*)
    Assign(f);
    // mov dword ptr [rbx + 8], 20 // `8` is a compile-time value.
    f->j0 = 20;
    // movabs rax, 171798691870 // 171798691870 == 0x280000001E, 0x28 == 40, 0x1E == 30
    // mov qword ptr [rbx + 24], rax // `24` is a compile-time value.
    f->j1 = 30;
    f->k = 40;
    // `j1` and `k` are adjacent in the layout.



    static_assert(sizeof(D0) == 32);
    D0 *d0 = new D0();
    // mov rax, qword ptr [rbx]
    // mov rcx, qword ptr [rax - 24]
    // mov dword ptr [rbx + rcx + 8], 10 // rcx==16 is a runtime value, `8` is a compile-time value.
    d0->i = 10;
    // mov rdi, qword ptr [rax - 24]
    // add rdi, rbx
    // call Assign(B*)
    Assign(d0);
    // mov dword ptr [rbx + 8], 20
    d0->j0 = 20;



    static_assert(sizeof(D1) == 32);
    D1 *d1 = new D1();
    // mov rax, qword ptr [rbx]
    // mov rcx, qword ptr [rax - 24]
    // mov dword ptr [rbx + rcx + 8], 10 // rcx==16 is a runtime value, `8` is a compile-time value.
    d1->i = 10;
    // mov rdi, qword ptr [rax - 24]
    // add rdi, rbx
    // call Assign(B*)
    Assign(d1);
    // mov dword ptr [rbx + 8], 30
    d1->j1 = 30;



    static_assert(sizeof(B) == 16);
    B *b = new B();
    // mov dword ptr [rbx + 8], 10 // `8` is a compile-time value.
    b->i = 10;
    // mov rdi, rbx
    // call Assign(B*)
    Assign(b);



    return 0;
}



/* clang++ -cc1 -fdump-record-layouts -O0 -std=c++17 virtual_base_class.cpp

*** Dumping AST Record Layout
         0 | class B
         0 |   (B vtable pointer)
         8 |   int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class D0
         0 |   (D0 vtable pointer)
         8 |   int j0
        16 |   class B (virtual base)
        16 |     (B vtable pointer)
        24 |     int i
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class D1
         0 |   (D1 vtable pointer)
         8 |   int j1
        16 |   class B (virtual base)
        16 |     (B vtable pointer)
        24 |     int i
           | [sizeof=32, dsize=28, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class F
         0 |   class D0 (primary base)
         0 |     (D0 vtable pointer)
         8 |     int j0
        16 |   class D1 (base)
        16 |     (D1 vtable pointer)
        24 |     int j1
        28 |   int k
        32 |   class B (virtual base)
        32 |     (B vtable pointer)
        40 |     int i
           | [sizeof=48, dsize=44, align=8,
           |  nvsize=32, nvalign=8]

*/



/* clang++ -cc1 -ast-dump -O0 -std=c++17 virtual_base_class.cpp

|-FunctionDecl 0x557125c25890 <line:29:1, line:32:1> line:29:6 used Assign 'void (B *)'
| |-ParmVarDecl 0x557125c257c8 <col:13, col:16> col:16 used b 'B *'
| `-CompoundStmt 0x557125c25ad8 <col:19, line:32:1>
|   |-BinaryOperator 0x557125c25a10 <line:30:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x557125c259b8 <col:5, col:8> 'int' lvalue ->i 0x557125bf1e20
|   | | `-ImplicitCastExpr 0x557125c259a0 <col:5> 'B *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x557125c25978 <col:5> 'B *' lvalue ParmVar 0x557125c257c8 'b' 'B *'
|   | `-IntegerLiteral 0x557125c259f0 <col:12> 'int' 10
|   `-CXXMemberCallExpr 0x557125c25ab0 <line:31:5, col:12> 'void'
|     `-MemberExpr 0x557125c25a78 <col:5, col:8> '<bound member function type>' ->Bar 0x557125bf1fb0
|       `-ImplicitCastExpr 0x557125c25a60 <col:5> 'B *' <LValueToRValue>
|         `-DeclRefExpr 0x557125c25a38 <col:5> 'B *' lvalue ParmVar 0x557125c257c8 'b' 'B *'
|-FunctionDecl 0x55639c2f3da8 <line:30:1, line:73:1> line:30:5 main 'int ()'
| `-CompoundStmt 0x55639c2fa730 <col:12, line:73:1>
|   |-DeclStmt 0x55639c2f4200 <line:31:5, col:35>
|   | `-StaticAssertDecl 0x55639c2f41c8 <col:5, col:34> col:5
|   |   |-BinaryOperator 0x55639c2f3ee0 <col:19, col:32> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x55639c2f3e88 <col:19, col:27> 'unsigned long' sizeof 'F'
|   |   | `-ImplicitCastExpr 0x55639c2f3ec8 <col:32> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x55639c2f3ea8 <col:32> 'int' 48
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x55639c2f9438 <line:32:5, col:19>
|   | `-VarDecl 0x55639c2f4228 <col:5, col:18> col:8 used f 'F *' cinit
|   |   `-CXXNewExpr 0x55639c2f93e0 <col:12, col:18> 'F *' Function 0x55639c2f4418 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x55639c2f93b0 <col:16, col:18> 'F' 'void () noexcept' zeroing
|   |-BinaryOperator 0x55639c2f9538 <line:38:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f94e0 <col:5, col:8> 'int' lvalue ->i 0x55639c2c0d70
|   | | `-ImplicitCastExpr 0x55639c2f94c0 <col:5> 'B *' <UncheckedDerivedToBase (virtual B)>
|   | |   `-ImplicitCastExpr 0x55639c2f94a8 <col:5> 'F *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x55639c2f9480 <col:5> 'F *' lvalue Var 0x55639c2f4228 'f' 'F *'
|   | `-IntegerLiteral 0x55639c2f9518 <col:12> 'int' 10
|   |-CallExpr 0x55639c2f9650 <line:39:5, col:13> 'void'
|   | |-ImplicitCastExpr 0x55639c2f9638 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x55639c2f95e0 <col:5> 'void (B *)' lvalue Function 0x55639c2f3ae0 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x55639c2f9698 <col:12> 'B *' <DerivedToBase (virtual B)>
|   |   `-ImplicitCastExpr 0x55639c2f9680 <col:12> 'F *' <LValueToRValue>
|   |     `-DeclRefExpr 0x55639c2f95b8 <col:12> 'F *' lvalue Var 0x55639c2f4228 'f' 'F *'
|   |-BinaryOperator 0x55639c2f9770 <line:43:5, col:13> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f9718 <col:5, col:8> 'int' lvalue ->j0 0x55639c2c1618
|   | | `-ImplicitCastExpr 0x55639c2f96f8 <col:5> 'D0 *' <UncheckedDerivedToBase (D0)>
|   | |   `-ImplicitCastExpr 0x55639c2f96e0 <col:5> 'F *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x55639c2f96b8 <col:5> 'F *' lvalue Var 0x55639c2f4228 'f' 'F *'
|   | `-IntegerLiteral 0x55639c2f9750 <col:13> 'int' 20
|   |-BinaryOperator 0x55639c2f9850 <line:47:5, col:13> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f97f8 <col:5, col:8> 'int' lvalue ->j1 0x55639c2f0fa8
|   | | `-ImplicitCastExpr 0x55639c2f97d8 <col:5> 'D1 *' <UncheckedDerivedToBase (D1)>
|   | |   `-ImplicitCastExpr 0x55639c2f97c0 <col:5> 'F *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x55639c2f9798 <col:5> 'F *' lvalue Var 0x55639c2f4228 'f' 'F *'
|   | `-IntegerLiteral 0x55639c2f9830 <col:13> 'int' 30
|   |-BinaryOperator 0x55639c2f9910 <line:49:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f98b8 <col:5, col:8> 'int' lvalue ->k 0x55639c2f1858
|   | | `-ImplicitCastExpr 0x55639c2f98a0 <col:5> 'F *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x55639c2f9878 <col:5> 'F *' lvalue Var 0x55639c2f4228 'f' 'F *'
|   | `-IntegerLiteral 0x55639c2f98f0 <col:12> 'int' 40
|   |-DeclStmt 0x55639c2f9a00 <line:53:5, col:36>
|   | `-StaticAssertDecl 0x55639c2f99c8 <col:5, col:35> col:5
|   |   |-BinaryOperator 0x55639c2f99a0 <col:19, col:33> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x55639c2f9948 <col:19, col:28> 'unsigned long' sizeof 'D0'
|   |   | `-ImplicitCastExpr 0x55639c2f9988 <col:33> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x55639c2f9968 <col:33> 'int' 32
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x55639c2f9b48 <line:54:5, col:22>
|   | `-VarDecl 0x55639c2f9a28 <col:5, col:21> col:9 used d0 'D0 *' cinit
|   |   `-CXXNewExpr 0x55639c2f9af0 <col:14, col:21> 'D0 *' Function 0x55639c2f4418 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x55639c2f9ac0 <col:18, col:21> 'D0' 'void () noexcept' zeroing
|   |-BinaryOperator 0x55639c2f9c18 <line:55:5, col:13> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f9bc0 <col:5, col:9> 'int' lvalue ->i 0x55639c2c0d70
|   | | `-ImplicitCastExpr 0x55639c2f9ba0 <col:5> 'B *' <UncheckedDerivedToBase (virtual B)>
|   | |   `-ImplicitCastExpr 0x55639c2f9b88 <col:5> 'D0 *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x55639c2f9b60 <col:5> 'D0 *' lvalue Var 0x55639c2f9a28 'd0' 'D0 *'
|   | `-IntegerLiteral 0x55639c2f9bf8 <col:13> 'int' 10
|   |-CallExpr 0x55639c2f9d00 <line:56:5, col:14> 'void'
|   | |-ImplicitCastExpr 0x55639c2f9ce8 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x55639c2f9cc0 <col:5> 'void (B *)' lvalue Function 0x55639c2f3ae0 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x55639c2f9d48 <col:12> 'B *' <DerivedToBase (virtual B)>
|   |   `-ImplicitCastExpr 0x55639c2f9d30 <col:12> 'D0 *' <LValueToRValue>
|   |     `-DeclRefExpr 0x55639c2f9c98 <col:12> 'D0 *' lvalue Var 0x55639c2f9a28 'd0' 'D0 *'
|   |-BinaryOperator 0x55639c2f9e00 <line:57:5, col:14> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2f9da8 <col:5, col:9> 'int' lvalue ->j0 0x55639c2c1618
|   | | `-ImplicitCastExpr 0x55639c2f9d90 <col:5> 'D0 *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x55639c2f9d68 <col:5> 'D0 *' lvalue Var 0x55639c2f9a28 'd0' 'D0 *'
|   | `-IntegerLiteral 0x55639c2f9de0 <col:14> 'int' 20
|   |-DeclStmt 0x55639c2f9ef0 <line:61:5, col:36>
|   | `-StaticAssertDecl 0x55639c2f9eb8 <col:5, col:35> col:5
|   |   |-BinaryOperator 0x55639c2f9e90 <col:19, col:33> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x55639c2f9e38 <col:19, col:28> 'unsigned long' sizeof 'D1'
|   |   | `-ImplicitCastExpr 0x55639c2f9e78 <col:33> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x55639c2f9e58 <col:33> 'int' 32
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x55639c2fa038 <line:62:5, col:22>
|   | `-VarDecl 0x55639c2f9f18 <col:5, col:21> col:9 used d1 'D1 *' cinit
|   |   `-CXXNewExpr 0x55639c2f9fe0 <col:14, col:21> 'D1 *' Function 0x55639c2f4418 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x55639c2f9fb0 <col:18, col:21> 'D1' 'void () noexcept' zeroing
|   |-BinaryOperator 0x55639c2fa108 <line:63:5, col:13> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2fa0b0 <col:5, col:9> 'int' lvalue ->i 0x55639c2c0d70
|   | | `-ImplicitCastExpr 0x55639c2fa090 <col:5> 'B *' <UncheckedDerivedToBase (virtual B)>
|   | |   `-ImplicitCastExpr 0x55639c2fa078 <col:5> 'D1 *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x55639c2fa050 <col:5> 'D1 *' lvalue Var 0x55639c2f9f18 'd1' 'D1 *'
|   | `-IntegerLiteral 0x55639c2fa0e8 <col:13> 'int' 10
|   |-CallExpr 0x55639c2fa1f0 <line:64:5, col:14> 'void'
|   | |-ImplicitCastExpr 0x55639c2fa1d8 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x55639c2fa1b0 <col:5> 'void (B *)' lvalue Function 0x55639c2f3ae0 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x55639c2fa238 <col:12> 'B *' <DerivedToBase (virtual B)>
|   |   `-ImplicitCastExpr 0x55639c2fa220 <col:12> 'D1 *' <LValueToRValue>
|   |     `-DeclRefExpr 0x55639c2fa188 <col:12> 'D1 *' lvalue Var 0x55639c2f9f18 'd1' 'D1 *'
|   |-BinaryOperator 0x55639c2fa2f0 <line:65:5, col:14> 'int' lvalue '='
|   | |-MemberExpr 0x55639c2fa298 <col:5, col:9> 'int' lvalue ->j1 0x55639c2f0fa8
|   | | `-ImplicitCastExpr 0x55639c2fa280 <col:5> 'D1 *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x55639c2fa258 <col:5> 'D1 *' lvalue Var 0x55639c2f9f18 'd1' 'D1 *'
|   | `-IntegerLiteral 0x55639c2fa2d0 <col:14> 'int' 30
|   |-DeclStmt 0x55639c2fa3e0 <line:69:5, col:35>
|   | `-StaticAssertDecl 0x55639c2fa3a8 <col:5, col:34> col:5
|   |   |-BinaryOperator 0x55639c2fa380 <col:19, col:32> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x55639c2fa328 <col:19, col:27> 'unsigned long' sizeof 'B'
|   |   | `-ImplicitCastExpr 0x55639c2fa368 <col:32> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x55639c2fa348 <col:32> 'int' 16
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x55639c2fa550 <line:70:5, col:19>
|   | `-VarDecl 0x55639c2fa408 <col:5, col:18> col:8 used b 'B *' cinit
|   |   `-CXXNewExpr 0x55639c2fa4f8 <col:12, col:18> 'B *' Function 0x55639c2f4418 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x55639c2fa4c8 <col:16, col:18> 'B' 'void () noexcept' zeroing
|   |-CallExpr 0x55639c2fa628 <line:71:5, col:13> 'void'
|   | |-ImplicitCastExpr 0x55639c2fa610 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x55639c2fa5e8 <col:5> 'void (B *)' lvalue Function 0x55639c2f3ae0 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x55639c2fa658 <col:12> 'B *' <LValueToRValue>
|   |   `-DeclRefExpr 0x55639c2fa5c0 <col:12> 'B *' lvalue Var 0x55639c2fa408 'b' 'B *'
|   `-BinaryOperator 0x55639c2fa708 <line:72:5, col:12> 'int' lvalue '='
|     |-MemberExpr 0x55639c2fa6b0 <col:5, col:8> 'int' lvalue ->i 0x55639c2c0d70
|     | `-ImplicitCastExpr 0x55639c2fa698 <col:5> 'B *' <LValueToRValue>
|     |   `-DeclRefExpr 0x55639c2fa670 <col:5> 'B *' lvalue Var 0x55639c2fa408 'b' 'B *'
|     `-IntegerLiteral 0x55639c2fa6e8 <col:12> 'int' 10

*/



/* https://cppinsights.io/    C++17

void Assign(B * b)
{
  b->i = 10;
  b->Bar();
}

int main()
{
  // PASSED: static_assert(sizeof(F) == 48);
  F * f = new F();
  static_cast<B *>(f)->i = 10;
  Assign(static_cast<B *>(f));
  static_cast<D0 *>(f)->j0 = 20;
  static_cast<D1 *>(f)->j1 = 30;
  f->k = 40;

  // PASSED: static_assert(sizeof(D0) == 32);
  D0 * d0 = new D0();
  static_cast<B *>(d0)->i = 10;
  Assign(static_cast<B *>(d0));
  d0->j0 = 20;

  // PASSED: static_assert(sizeof(D1) == 32);
  D1 * d1 = new D1();
  static_cast<B *>(d1)->i = 10;
  Assign(static_cast<B *>(d1));
  d1->j1 = 30;

  // PASSED: static_assert(sizeof(B) == 16);
  B * b = new B();
  Assign(b);
  b->i = 10;

  return 0;
}

*/



/* https://godbolt.org/    -Og -std=c++17

Assign(B*): # @Assign(B*)
  // b->i = 10;
  mov dword ptr [rdi + 8], 10

  // b->Bar();
  mov rax, qword ptr [rdi]
  // `Foo()` is at [rax + 0];
  // `Bar()` is at [rax + 8].
  jmp qword ptr [rax + 8] # TAILCALL

main: # @main
  push rbx

  // new(edi=48) -> rax=address
  mov edi, 48
  call operator new(unsigned long)

  // memset
  mov rbx, rax
  xorps xmm0, xmm0 // xmm0 has 128 bits, i.e. 16 bytes.
  // 16 * 3 = 48
  movaps xmmword ptr [rbx + 32], xmm0
  movaps xmmword ptr [rbx + 16], xmm0
  movaps xmmword ptr [rbx], xmm0

  // F::F()
  mov rdi, rbx
  call F::F() [complete object constructor]

  // f->i = 10;
  mov rax, qword ptr [rbx]
  mov rcx, qword ptr [rax - 24]
  mov dword ptr [rbx + rcx + 8], 10

  // Assign(f);
  mov rdi, qword ptr [rax - 24]
  add rdi, rbx
  call Assign(B*)

  // f->j0 = 20;
  mov dword ptr [rbx + 8], 20

  // f->j1 = 30; f->k = 40;
  movabs rax, 171798691870 // 171798691870 == 0x280000001E, 0x28 == 40, 0x1E == 30
  mov qword ptr [rbx + 24], rax

  // new(edi=32) -> rax=address
  mov edi, 32
  call operator new(unsigned long)

  // memset
  mov rbx, rax
  xorps xmm0, xmm0 // xmm0 has 128 bits, i.e. 16 bytes.
  // 16 * 2 = 32
  movaps xmmword ptr [rbx + 16], xmm0
  movaps xmmword ptr [rbx], xmm0

  // D0::D0()
  mov rdi, rbx
  call D0::D0() [complete object constructor]

  // d0->i = 10;
  mov rax, qword ptr [rbx]
  mov rcx, qword ptr [rax - 24]
  mov dword ptr [rbx + rcx + 8], 10

  // Assign(d0);
  mov rdi, qword ptr [rax - 24]
  add rdi, rbx
  call Assign(B*)

  // d0->j0 = 20;
  mov dword ptr [rbx + 8], 20

  // new(edi=32) -> rax=address
  mov edi, 32
  call operator new(unsigned long)

  // memset
  mov rbx, rax
  xorps xmm0, xmm0 // xmm0 has 128 bits, i.e. 16 bytes.
  // 16 * 2 = 32
  movaps xmmword ptr [rbx + 16], xmm0
  movaps xmmword ptr [rbx], xmm0

  // D1::D1()
  mov rdi, rbx
  call D1::D1() [complete object constructor]

  // d1->i = 10;
  mov rax, qword ptr [rbx]
  mov rcx, qword ptr [rax - 24]
  mov dword ptr [rbx + rcx + 8], 10

  // Assign(d1);
  mov rdi, qword ptr [rax - 24]
  add rdi, rbx
  call Assign(B*)

  // d1->j1 = 30;
  mov dword ptr [rbx + 8], 30

  // new(edi=16) -> rax=address
  mov edi, 16
  call operator new(unsigned long)

  // memset
  mov rbx, rax
  xorps xmm0, xmm0 // xmm0 has 128 bits, i.e. 16 bytes.
  // 16 * 1 = 16
  movaps xmmword ptr [rbx], xmm0

  // B::B()
  mov rdi, rbx
  call B::B() [base object constructor]

  // b->i = 10;
  mov dword ptr [rbx + 8], 10

  // Assign(b);
  mov rdi, rbx
  call Assign(B*)

  // return
  xor eax, eax
  pop rbx
  ret

F::F() [complete object constructor]: # @F::F() [complete object constructor]
  push rbx
  mov rbx, rdi
  lea rdi, [rbx + 32]
  call B::B() [base object constructor]
  mov esi, offset VTT for F+8
  mov rdi, rbx
  call D0::D0() [base object constructor]
  lea rdi, [rbx + 16]
  mov esi, offset VTT for F+24
  call D1::D1() [base object constructor]
  mov qword ptr [rbx], offset vtable for F+24
  mov qword ptr [rbx + 32], offset vtable for F+112
  mov qword ptr [rbx + 16], offset vtable for F+64
  mov dword ptr [rbx + 28], 4
  pop rbx
  ret
D0::D0() [complete object constructor]: # @D0::D0() [complete object constructor]
  push rbx
  mov rbx, rdi
  lea rdi, [rbx + 16]
  call B::B() [base object constructor]
  mov qword ptr [rbx], offset vtable for D0+24
  mov qword ptr [rbx + 16], offset vtable for D0+72
  mov dword ptr [rbx + 8], 2
  pop rbx
  ret
D1::D1() [complete object constructor]: # @D1::D1() [complete object constructor]
  push rbx
  mov rbx, rdi
  lea rdi, [rbx + 16]
  call B::B() [base object constructor]
  mov qword ptr [rbx], offset vtable for D1+24
  mov qword ptr [rbx + 16], offset vtable for D1+72
  mov dword ptr [rbx + 8], 3
  pop rbx
  ret
B::B() [base object constructor]: # @B::B() [base object constructor]
  mov qword ptr [rdi], offset vtable for B+16
  mov dword ptr [rdi + 8], 1
  ret
D0::D0() [base object constructor]: # @D0::D0() [base object constructor]
  mov rax, qword ptr [rsi]
  mov qword ptr [rdi], rax
  mov rcx, qword ptr [rsi + 8]
  mov rax, qword ptr [rax - 24]
  mov qword ptr [rdi + rax], rcx
  mov dword ptr [rdi + 8], 2
  ret
D1::D1() [base object constructor]: # @D1::D1() [base object constructor]
  mov rax, qword ptr [rsi]
  mov qword ptr [rdi], rax
  mov rcx, qword ptr [rsi + 8]
  mov rax, qword ptr [rax - 24]
  mov qword ptr [rdi + rax], rcx
  mov dword ptr [rdi + 8], 3
  ret
D0::Foo(): # @D0::Foo()
  ret
D0::Bar(): # @D0::Bar()
  ret
virtual thunk to D0::Foo(): # @virtual thunk to D0::Foo()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 24]
  jmp D0::Foo() # TAILCALL
virtual thunk to D0::Bar(): # @virtual thunk to D0::Bar()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 32]
  jmp D0::Bar() # TAILCALL
D1::Foo(): # @D1::Foo()
  ret
D1::Bar(): # @D1::Bar()
  ret
virtual thunk to D1::Foo(): # @virtual thunk to D1::Foo()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 24]
  jmp D1::Foo() # TAILCALL
virtual thunk to D1::Bar(): # @virtual thunk to D1::Bar()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 32]
  jmp D1::Bar() # TAILCALL
F::Foo(): # @F::Foo()
  ret
F::Bar(): # @F::Bar()
  ret
non-virtual thunk to F::Foo(): # @non-virtual thunk to F::Foo()
  add rdi, -16
  jmp F::Foo() # TAILCALL
non-virtual thunk to F::Bar(): # @non-virtual thunk to F::Bar()
  add rdi, -16
  jmp F::Bar() # TAILCALL
virtual thunk to F::Foo(): # @virtual thunk to F::Foo()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 24]
  jmp F::Foo() # TAILCALL
virtual thunk to F::Bar(): # @virtual thunk to F::Bar()
  mov rax, qword ptr [rdi]
  add rdi, qword ptr [rax - 32]
  jmp F::Bar() # TAILCALL
B::Foo(): # @B::Foo()
  ret
B::Bar(): # @B::Bar()
  ret
vtable for F:
  .quad 32
  .quad 0
  .quad typeinfo for F
  .quad F::Foo()
  .quad F::Bar()
  .quad 16
  .quad -16
  .quad typeinfo for F
  .quad non-virtual thunk to F::Foo()
  .quad non-virtual thunk to F::Bar()
  .quad -32
  .quad -32
  .quad -32
  .quad typeinfo for F
  .quad virtual thunk to F::Foo()
  .quad virtual thunk to F::Bar()

VTT for F:
  .quad vtable for F+24
  .quad construction vtable for D0-in-F+24
  .quad construction vtable for D0-in-F+72
  .quad construction vtable for D1-in-F+24
  .quad construction vtable for D1-in-F+72
  .quad vtable for F+112
  .quad vtable for F+64

construction vtable for D0-in-F:
  .quad 32
  .quad 0
  .quad typeinfo for D0
  .quad D0::Foo()
  .quad D0::Bar()
  .quad -32
  .quad -32
  .quad -32
  .quad typeinfo for D0
  .quad virtual thunk to D0::Foo()
  .quad virtual thunk to D0::Bar()

typeinfo name for D0:
  .asciz "2D0"

typeinfo name for B:
  .asciz "1B"

typeinfo for B:
  .quad vtable for __cxxabiv1::__class_type_info+16
  .quad typeinfo name for B

typeinfo for D0:
  .quad vtable for __cxxabiv1::__vmi_class_type_info+16
  .quad typeinfo name for D0
  .long 0 # 0x0
  .long 1 # 0x1
  .quad typeinfo for B
  .quad -6141 # 0xffffffffffffe803

construction vtable for D1-in-F:
  .quad 16
  .quad 0
  .quad typeinfo for D1
  .quad D1::Foo()
  .quad D1::Bar()
  .quad -16
  .quad -16
  .quad -16
  .quad typeinfo for D1
  .quad virtual thunk to D1::Foo()
  .quad virtual thunk to D1::Bar()

typeinfo name for D1:
  .asciz "2D1"

typeinfo for D1:
  .quad vtable for __cxxabiv1::__vmi_class_type_info+16
  .quad typeinfo name for D1
  .long 0 # 0x0
  .long 1 # 0x1
  .quad typeinfo for B
  .quad -6141 # 0xffffffffffffe803

typeinfo name for F:
  .asciz "1F"

typeinfo for F:
  .quad vtable for __cxxabiv1::__vmi_class_type_info+16
  .quad typeinfo name for F
  .long 2 # 0x2
  .long 2 # 0x2
  .quad typeinfo for D0
  .quad 2 # 0x2
  .quad typeinfo for D1
  .quad 4098 # 0x1002

vtable for B:
  .quad 0
  .quad typeinfo for B
  .quad B::Foo()
  .quad B::Bar()

vtable for D0:
  .quad 16
  .quad 0
  .quad typeinfo for D0
  .quad D0::Foo()
  .quad D0::Bar()
  .quad -16
  .quad -16
  .quad -16
  .quad typeinfo for D0
  .quad virtual thunk to D0::Foo()
  .quad virtual thunk to D0::Bar()

vtable for D1:
  .quad 16
  .quad 0
  .quad typeinfo for D1
  .quad D1::Foo()
  .quad D1::Bar()
  .quad -16
  .quad -16
  .quad -16
  .quad typeinfo for D1
  .quad virtual thunk to D1::Foo()
  .quad virtual thunk to D1::Bar()

*/
