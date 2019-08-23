#include <cassert>
#include <vector>
using namespace std;

class DisjointSets {
    std::vector<int> parent_;
    std::vector<int> rank_;

    int Find(int x) {
        if(parent_[x] == x)
            return x;
        else
            return parent_[x] = Find(parent_[x]);
    }

public:
    // Indexes start from 0 or 1.
    DisjointSets(int size) : parent_(size + 1), rank_(size + 1) {
        for(int i = 0; i <= size; i++)
            parent_[i] = i;
    }

    void Unite(int x0, int x1) {
        int f0 = Find(x0);
        int f1 = Find(x1);
        if(rank_[f0] > rank_[f1]) {
            parent_[f1] = f0;
        } else {
            parent_[f0] = f1;
            if(rank_[f0] == rank_[f1])
                rank_[f1]++;
        }
    }

    bool IsSame(int x0, int x1) {
        return Find(x0) == Find(x1);
    }
};

int main() {
    vector<vector<int>> sets = {
        {0, 3, 6},
        {1, 4, 7},
        {2, 5},
    };

    DisjointSets ds(8);
    for(auto &s: sets)
        for(size_t i = 1; i < s.size(); i++)
            ds.Unite(s[0], s[i]);

    assert(ds.IsSame(0, 6) == true);
    assert(ds.IsSame(0, 1) == false);

    return 0;
}
