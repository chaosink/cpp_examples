#include "common.hpp"

struct S {
    // We want to use operator comma like this:
    //     s << 0, 1, 2, 3, 4;
    // but not like this:
    //     s, 0, 1, 2, 3, 4;
    // So we overload operator comma for an embedded class SS
    // but not for S.
    struct SS {
        vector<int> data;

        SS &operator,(int i) {
            data.push_back(i);
            return *this;
        }
    };
    SS ss;

    SS &operator<<(int i) {
        return ss, i;
    }

    auto begin() {
        return ss.data.begin();
    }
    auto end() {
        return ss.data.end();
    }
};

int main() {
    S s;
    s << 0, 1, 2, 3, 4;
    int k = 0;
    for(auto i: s)
        assert(i == k++);
}
