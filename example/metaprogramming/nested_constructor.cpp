template <typename T>
struct A {
   T v;

   A(T v) : v{v} {}
};

template <typename T>
struct B {
   T v;

   B(T v) : v{v} {}

   template <typename S>
   B(S v) : v{v} {}
};

int main() {
    A<int>{0};             // OK.
    A<A<int>>{0};          // OK.
                           // `int` can be cast to `A<int>` implicitly with A<int>(int)
    // A<A<A<int>>>{0};    // Error.
                           // Only one implicit conversion is allowed.
                           // `int` can convert to `A<A<int>>` only by `A<A<int>>(int)`.
                           // But there is no `A<A<int>>(int)`.
                           // Only `A<A<int>>(A<int>)`.
    // A<A<A<A<int>>>>{0}; // Error.
    // ...                 // Error...


    B<int>{0};          // OK.
    B<B<int>>{0};       // OK.
    B<B<B<int>>>{0};    // OK.
                        // `B<B<B<int>>>` has the constructor `B<B<B<int>>>(int)` due to
                        // the templated constructor, which calls `B<B<int>>(int)` in the
                        // member initializer list.
    B<B<B<B<int>>>>{0}; // OK
    // ...              // OK...

    return 0;
}
