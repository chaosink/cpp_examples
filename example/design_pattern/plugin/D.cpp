#include "B.hpp"

// x, D::y, z are initialized in order when loading D.so.
int x = [](){
	std::cout << "x    10" << std::endl;
	return 10;
}();

class D: public B {
	static int y;
public:
	virtual ~D() {
		std::cout << "~D()" << std::endl;
	}
	virtual void Print() override {
		std::cout << "D::y " << y  << std::endl;
	}
};
int D::y = [](){
	std::cout << "D::y 20" << std::endl;
	return 20;
}();

int z = [](){
	std::cout << "z    30" << std::endl;
	return 30;
}();

extern "C" {
void* CreateInstance() {
	return new D;
}
}
