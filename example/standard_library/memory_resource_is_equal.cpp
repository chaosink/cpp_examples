#include <array>
#include <iostream>
#include <memory_resource>
#include <string>
using namespace std;

class Tracker: public pmr::memory_resource {
public:
    pmr::memory_resource *upstream;
    string prefix{};

public:
    explicit Tracker(pmr::memory_resource *us = pmr::get_default_resource()): upstream{us} {}
    explicit Tracker(string p, pmr::memory_resource *us = pmr::get_default_resource())
        : prefix{move(p)}, upstream{us} {}

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *ret = upstream->allocate(bytes, alignment);
        cout << prefix << " allocate " << bytes << " bytes at " << ret << endl;
        return ret;
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        cout << prefix << " deallocate " << bytes << " bytes at " << ptr << endl;
        upstream->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const pmr::memory_resource &other) const noexcept override {
        return this == &other || dynamic_cast<const Tracker *>(&other) != nullptr;
    }
};

int main() {
    array<byte, 1024> mem1;
    pmr::monotonic_buffer_resource mbr1(&mem1, 1024);
    Tracker track1{"track1:", &mbr1};
    array<byte, 1024> mem2;
    pmr::monotonic_buffer_resource mbr2(&mem2, 1024);
    Tracker track2{"track2:", &mbr2};

    cout << "mem1: " << &mem1 << endl;
    cout << "mem2: " << &mem2 << endl;

    cout << endl;

    pmr::vector<int> i0{2, &track1};
    cout << "i0: " << (uint64_t *)i0.data() << endl;
    i0.emplace_back(1);

    cout << endl;

    pmr::vector<int> i1{2, &track1};
    cout << "i1: " << (uint64_t *)i1.data() << endl;
    // `do_is_equal()` returns true, so `i2`'s data is still in `mem1`, and the following allocations/deallocations for `i2`'s memory are done via `track2`.
    pmr::vector<int> i2{move(i1), &track2};
    cout << "i2: " << (uint64_t *)i2.data() << endl;
    // Memory exhausted. To emplace the new element, first `i2`'s old memory in `mem1` is released by `track2`, then new larger memory is allocated by `track2` in `mem2`.
    i2.emplace_back(1);

    cout << endl;

    return 0;
}
