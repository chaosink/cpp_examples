#include "common.hpp"

#include <dlfcn.h>

#include "B.hpp"

int main() {
	B *b = new B;
	b->Print();
	cout << endl;



	void *handle = dlopen("D.so", RTLD_LAZY | RTLD_LOCAL);
	if(!handle) return 1;
	cout << endl;



	void *x = dlsym(handle, "x");
	if(x)
		cout << "x    " << *reinterpret_cast<int*>(x) << endl;

	void *s = dlsym(handle, "CreateInstance");
	B *d = nullptr;
	if(s) {
		d = reinterpret_cast<B*(*)()>(s)();
		d->Print();
	}

	void *z = dlsym(handle, "z");
	if(z)
		cout << "z    " << *reinterpret_cast<int*>(z) << endl;

	cout << endl;



	delete d;
	cout << endl;
	delete b;

	dlclose(handle);

	// delete d; // Error!
	// After closing .so, the virtual function table and the virtual destructor is not accessible.
}
