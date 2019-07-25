#include <iostream>

class B {
public:
	virtual ~B() {
		std::cout << "~B()" << std::endl;
	}
	virtual void Print() {
		std::cout << "B" << std::endl;
	}
};
