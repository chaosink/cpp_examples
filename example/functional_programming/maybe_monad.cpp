#include <iostream>
#include <memory>
using namespace std;

struct Address {
    shared_ptr<string> house_name = nullptr;
};

struct Person {
    shared_ptr<Address> address = nullptr;
};

template<typename T>
struct Maybe;

template<typename T>
Maybe<T> maybe(T *context) {
    return Maybe<T>(context);
}

template<typename T>
struct Maybe {
    T *context;
    Maybe(T *context) : context(context) {}

    template<typename Func>
    auto With(Func evaluator) {
        return context != nullptr ? maybe(evaluator(context)) : nullptr;
    }

    template<typename TFunc>
    auto Do(TFunc action) {
        if(context != nullptr)
            action(context);
        return *this;
    }
};

void print_house_name(Person *p) {
    maybe(p)
        .With([](auto x) { return x->address.get(); })
        .With([](auto x) { return x->house_name.get(); })
        .Do([](auto x) { cout << *x << endl; });
}

int main() {
    shared_ptr<Person> p0{new Person{
        shared_ptr<Address>{new Address{
            shared_ptr<string>{new string{"123"}}
        }}
    }};
    print_house_name(p0.get()); // output: 123

    shared_ptr<Person> p1{new Person{
        shared_ptr<Address>{new Address}
    }};
    print_house_name(p1.get()); // no output
}
