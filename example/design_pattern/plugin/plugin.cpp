#include <iostream>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "B.hpp"

int main(int /*argc*/, char *argv[]) {
    B *b = new B;
    b->Print();
    std::cout << std::endl;



    std::string lib_path = argv[0];
#if defined(_WIN32)
    lib_path.replace(lib_path.rfind("plugin.exe"), -1, "D.dll");
    HMODULE handle = LoadLibrary(lib_path.c_str());
#else
    lib_path.replace(lib_path.rfind("plugin"), -1, "D.so");
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif
    if(!handle)
        return 1;
    std::cout << std::endl;

#if defined(_WIN32)
    void *x = GetProcAddress(handle, "x");
#else
    void *x = dlsym(handle, "x");
#endif
    if(x)
        std::cout << "x    " << *reinterpret_cast<int *>(x) << std::endl;

#if defined(_WIN32)
    void *s = GetProcAddress(handle, "CreateInstance");
#else
    void *s = dlsym(handle, "CreateInstance");
#endif
    B *d = nullptr;
    if(s) {
        d = reinterpret_cast<B *(*)()>(s)();
        d->Print();
    }

#if defined(_WIN32)
    void *z = GetProcAddress(handle, "z");
#else
    void *z = dlsym(handle, "z");
#endif
    if(z)
        std::cout << "z    " << *reinterpret_cast<int *>(z) << std::endl;

    std::cout << std::endl;



    delete d;
    std::cout << std::endl;
    delete b;

#if defined(_WIN32)
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif

    // delete d; // Error!
    // After closing .so, the virtual function table and the virtual destructor is not accessible.
}
