#include <iostream>
using namespace std;

struct Address {
	string* house_name = nullptr;
};

struct Person {
	Address* address = nullptr;
};

template <typename T> struct Maybe;

template <typename T> Maybe<T> maybe(T* context) {
	return Maybe<T>(context);
}

template <typename T> struct Maybe {
	T* context;
	Maybe(T *context): context(context) {}

	template <typename Func>
	auto With(Func evaluator) {
		return context != nullptr ? maybe(evaluator(context)) : nullptr;
	}

	template <typename TFunc>
	auto Do(TFunc action) {
		if(context != nullptr) action(context);
		return *this;
	}
};

void print_house_name(Person* p) {
	maybe(p)
		.With([](auto x) { return x->address; })
		.With([](auto x) { return x->house_name; })
		.Do([](auto x) { cout << *x << endl; });
}

int main() {
	Person *p = new Person{new Address{new string{"123"}}};
	print_house_name(p);
}
