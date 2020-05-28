// CRTP (Curiously Recurring Template Pattern)
// 静态函数和模板函数不能是虚函数
// 编译期可用CRTP模拟虚函数

#include <cassert>

template<typename D>
struct B {
    D *Cast() {
        return static_cast<D*>(this); // 需用`static_cast<D*>`显示转换类型
    }

    // 虚静态函数模板
    template<typename T>
    constexpr static T Fun0(const T &a) { // Fun0和Imp0不同名
        return D::Imp0(a);
    }
    template<typename T>
    constexpr static T Imp0(const T &a) { // 无法调用
        return a;
    }

    // 虚函数模板
    template<typename T>
    T Fun1(const T &a) { // Fun1和Imp1不同名
        return Cast()->Imp1(a);
    }
    template<typename T>
    T Imp1(const T &a) { // 无法调用
        return a;
    }
};

struct D0 : public B<D0> {
    // 虚静态函数模板
    template<typename T>
    constexpr static T Imp0(const T &a) {
        return a * 2;
    }

    // 虚函数模板
    template<typename T>
    T Imp1(const T &a) {
        return a * 2;
    }
};

struct D1 : public B<D1> {
    // 虚静态函数模板
    template<typename T>
    constexpr static T Imp0(const T &a) {
        return a * 3;
    }

    // 虚函数模板
    template<typename T>
    T Imp1(const T &a) {
        return a * 3;
    }
};

int main() {
    // B::Fun0(1);    // Compilation error.
    // B<B>::Fun0(1); // Compilation error.
    static_assert(D0::Fun0(1) == 2);
    static_assert(D1::Fun0(1) == 3);



    // B b0;    // Compilation error.
    // B<B> b1; // Compilation error.

    D0 d0;
    assert(d0.Fun1(1) == 2);

    D1 d1;
    assert(d1.Fun1(1) == 3);
}
