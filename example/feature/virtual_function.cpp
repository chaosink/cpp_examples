class B {
public:
    int i = 1;
    virtual void Foo() {}
    virtual void Bar() {}
};

class D: public B {
public:
    int j = 2;
    virtual void Foo() {}
    virtual void Bar() {}
};

void Assign(B *b) {
    // mov     DWORD PTR [rdi+8], 10
    b->i = 10;

    // mov     rax, QWORD PTR [rdi]
    // call    [QWORD PTR [rax+8]] // `8` is a compile-time value.
    b->Bar();
    // `Foo()` is at [rax+0];
    // `Bar()` is at [rax+8].
}

int main() {
    static_assert(sizeof(D) == 16);
    D *d = new D();
    // mov     DWORD PTR [rax+8], 10 // `8` is a compile-time value.
    d->i = 10;
    // mov     rdi, rax
    // call    Assign(B*)
    Assign(d);
    // mov     DWORD PTR [rbx+12], 30
    d->j = 30;



    static_assert(sizeof(B) == 16);
    B *b = new B();
    // mov     DWORD PTR [rax+8], 10 // `8` is a compile-time value.
    b->i = 10;
    // call    Assign(B*)
    Assign(b);



    return 0;
}

/*------------------------------------------------------------------------------------------------*/
/* clang++ -cc1 -fdump-record-layouts -O0 -std=c++17 virtual_function.cpp

*** Dumping AST Record Layout
         0 | class B
         0 |   (B vtable pointer)
         8 |   int i
           | [sizeof=16, dsize=12, align=8,
           |  nvsize=12, nvalign=8]

*** Dumping AST Record Layout
         0 | class D
         0 |   class B (primary base)
         0 |     (B vtable pointer)
         8 |     int i
        12 |   int j
           | [sizeof=16, dsize=16, align=8,
           |  nvsize=16, nvalign=8]

*/

/*------------------------------------------------------------------------------------------------*/
/* clang++ -cc1 -ast-dump -O0 -std=c++17 virtual_function.cpp

|-FunctionDecl 0x1460b68 <line:15:1, line:24:1> line:15:6 used Assign 'void (B *)'
| |-ParmVarDecl 0x1460aa0 <col:13, col:16> col:16 used b 'B *'
| `-CompoundStmt 0x1460d88 <col:19, line:24:1>
|   |-BinaryOperator 0x1460ce0 <line:17:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x1460c90 <col:5, col:8> 'int' lvalue ->i 0x1431388
|   | | `-ImplicitCastExpr 0x1460c78 <col:5> 'B *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x1460c58 <col:5> 'B *' lvalue ParmVar 0x1460aa0 'b' 'B *'
|   | `-IntegerLiteral 0x1460cc0 <col:12> 'int' 10
|   `-CXXMemberCallExpr 0x1460d68 <line:21:5, col:12> 'void'
|     `-MemberExpr 0x1460d38 <col:5, col:8> '<bound member function type>' ->Bar 0x1431518
|       `-ImplicitCastExpr 0x1460d20 <col:5> 'B *' <LValueToRValue>
|         `-DeclRefExpr 0x1460d00 <col:5> 'B *' lvalue ParmVar 0x1460aa0 'b' 'B *'
|-FunctionDecl 0x1460e00 <line:75:1, line:102:1> line:75:5 main 'int ()'
| `-CompoundStmt 0x1464100 <col:12, line:102:1>
|   |-DeclStmt 0x1461120 <line:76:5, col:35>
|   | `-StaticAssertDecl 0x14610e0 <col:5, col:34> col:5
|   |   |-BinaryOperator 0x1460f30 <col:19, col:32> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x1460ed8 <col:19, col:27> 'unsigned long' sizeof 'D'
|   |   | `-ImplicitCastExpr 0x1460f18 <col:32> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x1460ef8 <col:32> 'int' 16
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x1463a78 <line:77:5, col:19>
|   | `-VarDecl 0x1461148 <col:5, col:18> col:8 used d 'D *' cinit
|   |   `-CXXNewExpr 0x1463a38 <col:12, col:18> 'D *' Function 0x1461340 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x1463a10 <col:16, col:18> 'D' 'void () noexcept' zeroing
|   |-BinaryOperator 0x1463b38 <line:81:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x1463ae8 <col:5, col:8> 'int' lvalue ->i 0x1431388
|   | | `-ImplicitCastExpr 0x1463ac8 <col:5> 'B *' <UncheckedDerivedToBase (B)>
|   | |   `-ImplicitCastExpr 0x1463ab0 <col:5> 'D *' <LValueToRValue>
|   | |     `-DeclRefExpr 0x1463a90 <col:5> 'D *' lvalue Var 0x1461148 'd' 'D *'
|   | `-IntegerLiteral 0x1463b18 <col:12> 'int' 10
|   |-CallExpr 0x1463c20 <line:85:5, col:13> 'void'
|   | |-ImplicitCastExpr 0x1463c08 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x1463bc0 <col:5> 'void (B *)' lvalue Function 0x1460b68 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x1463c60 <col:12> 'B *' <DerivedToBase (B)>
|   |   `-ImplicitCastExpr 0x1463c48 <col:12> 'D *' <LValueToRValue>
|   |     `-DeclRefExpr 0x1463ba0 <col:12> 'D *' lvalue Var 0x1461148 'd' 'D *'
|   |-BinaryOperator 0x1463d08 <line:87:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x1463cb8 <col:5, col:8> 'int' lvalue ->j 0x1431ca8
|   | | `-ImplicitCastExpr 0x1463ca0 <col:5> 'D *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x1463c80 <col:5> 'D *' lvalue Var 0x1461148 'd' 'D *'
|   | `-IntegerLiteral 0x1463ce8 <col:12> 'int' 30
|   |-DeclStmt 0x1463e10 <line:91:5, col:35>
|   | `-StaticAssertDecl 0x1463dd0 <col:5, col:34> col:5
|   |   |-BinaryOperator 0x1463d90 <col:19, col:32> 'bool' '=='
|   |   | |-UnaryExprOrTypeTraitExpr 0x1463d38 <col:19, col:27> 'unsigned long' sizeof 'B'
|   |   | `-ImplicitCastExpr 0x1463d78 <col:32> 'unsigned long' <IntegralCast>
|   |   |   `-IntegerLiteral 0x1463d58 <col:32> 'int' 16
|   |   `-<<<NULL>>>
|   |-DeclStmt 0x1463f30 <line:92:5, col:19>
|   | `-VarDecl 0x1463e38 <col:5, col:18> col:8 used b 'B *' cinit
|   |   `-CXXNewExpr 0x1463ef0 <col:12, col:18> 'B *' Function 0x1461340 'operator new' 'void *(unsigned long)'
|   |     `-CXXConstructExpr 0x1463ec8 <col:16, col:18> 'B' 'void () noexcept' zeroing
|   |-BinaryOperator 0x1463fd0 <line:94:5, col:12> 'int' lvalue '='
|   | |-MemberExpr 0x1463f80 <col:5, col:8> 'int' lvalue ->i 0x1431388
|   | | `-ImplicitCastExpr 0x1463f68 <col:5> 'B *' <LValueToRValue>
|   | |   `-DeclRefExpr 0x1463f48 <col:5> 'B *' lvalue Var 0x1463e38 'b' 'B *'
|   | `-IntegerLiteral 0x1463fb0 <col:12> 'int' 10
|   |-CallExpr 0x1464090 <line:97:5, col:13> 'void'
|   | |-ImplicitCastExpr 0x1464078 <col:5> 'void (*)(B *)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x1464058 <col:5> 'void (B *)' lvalue Function 0x1460b68 'Assign' 'void (B *)'
|   | `-ImplicitCastExpr 0x14640b8 <col:12> 'B *' <LValueToRValue>
|   |   `-DeclRefExpr 0x1464038 <col:12> 'B *' lvalue Var 0x1463e38 'b' 'B *'
|   `-ReturnStmt 0x14640f0 <line:101:5, col:12>
|     `-IntegerLiteral 0x14640d0 <col:12> 'int' 0

*/



/* https://cppinsights.io/    C++17

void Assign(B * b)
{
  b->i = 10;
  b->Bar();
}


int main()
{
  // PASSED: static_assert(sizeof(D) == 16);
  D * d = new D();
  static_cast<B *>(d)->i = 10;
  Assign(static_cast<B *>(d));
  d->j = 30;

  // PASSED: static_assert(sizeof(B) == 16);
  B * b = new B();
  b->i = 10;
  Assign(b);

  return 0;
}

*/



/* https://godbolt.org/    -Og -std=c++17

Assign(B*):
        sub     rsp, 8

        // b->i = 10;
        mov     DWORD PTR [rdi+8], 10

        // b->Bar();
        mov     rax, QWORD PTR [rdi]
        call    [QWORD PTR [rax+8]] // `8` is a compile-time value.
        // `Foo()` is at [rax+0];
        // `Bar()` is at [rax+8].

        add     rsp, 8
        ret

main:
        push    rbx

        // new(edi=16) -> rax=address
        mov     edi, 16
        call    operator new(unsigned long)

        mov     rbx, rax

        // memset(rax, 0, 16)
        mov     QWORD PTR [rax], 0    // 8 bytes for `B` vtable pointer.
        mov     DWORD PTR [rax+8], 0  // 4 bytes for `B::i`.
        mov     DWORD PTR [rax+12], 0 // 4 bytes for `D::j`.

        // B vtable pointer.
        mov     QWORD PTR [rax], OFFSET FLAT:vtable for B+16

        // B::i = 1;
        mov     DWORD PTR [rax+8], 1

        // D vtable pointer, overwriting [rax].
        mov     QWORD PTR [rax], OFFSET FLAT:vtable for D+16

        // D::j = 2;
        mov     DWORD PTR [rax+12], 2

        // d->i = 10;
        mov     DWORD PTR [rax+8], 10

        // Assign(d);
        mov     rdi, rax
        call    Assign(B*)

        // d->j = 30;
        mov     DWORD PTR [rbx+12], 30



        // new(edi=16) -> rax=address
        mov     edi, 16
        call    operator new(unsigned long)

        mov     rdi, rax

        // memset(rax, 0, 12)
        mov     QWORD PTR [rax], 0   // 8 bytes for `B` vtable pointer.
        mov     DWORD PTR [rax+8], 0 // 4 bytes for `B::i`.

        // B vtable pointer.
        mov     QWORD PTR [rax], OFFSET FLAT:vtable for B+16

        // B::i = 1;
        mov     DWORD PTR [rax+8], 1

        // b->i = 10;
        mov     DWORD PTR [rax+8], 10

        // Assign(b);
        call    Assign(B*)



        // return 0
        mov     eax, 0
        pop     rbx
        ret

typeinfo name for B:
        .string "1B"

typeinfo for B:
        .quad   vtable for __cxxabiv1::__class_type_info+16
        .quad   typeinfo name for B

typeinfo name for D:
        .string "1D"

typeinfo for D:
        .quad   vtable for __cxxabiv1::__si_class_type_info+16
        .quad   typeinfo name for D
        .quad   typeinfo for B

vtable for B:
        .quad   0
        .quad   typeinfo for B
        .quad   B::Foo()
        .quad   B::Bar()

vtable for D:
        .quad   0
        .quad   typeinfo for D
        .quad   D::Foo()
        .quad   D::Bar()

*/
