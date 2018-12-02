#include "B.hpp"

int x = [](){
	std::cout << "x 10" << std::endl;
	return 10;
}();

class D: public B {
public:
	static int y;
	void Print() override {
		std::cout << "D" << std::endl;
	}
};
int D::y = [](){
	std::cout << "y 20" << std::endl;
	return 20;
}();

extern "C" {
void* CreateInstance() {
	return new D;
}
}
