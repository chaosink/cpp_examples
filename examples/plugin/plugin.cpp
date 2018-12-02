#include "common.hpp"

#include <dlfcn.h>

#include "B.hpp"

int main() {
	void *handle = dlopen("D.so", RTLD_LAZY | RTLD_LOCAL);
	if(!handle) return 1;

	void *x = dlsym(handle, "x");
	if(x) {
		cout << *reinterpret_cast<int*>(x) << endl;
	}

	void *s = dlsym(handle, "createInstance");
	if(s) {
		B *d = reinterpret_cast<B*(*)()>(s)();
		d->Print();
	}

	dlclose(handle);
}
