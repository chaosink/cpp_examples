// Kruskal
// A minimum-spanning-tree algorithm.

#include <cassert>

#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int a, b, d;
    bool operator<(const Edge &e) {
        return d < e.d;
    }
};

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

int Kruskal(int n, const vector<Edge> &_edges) {
    DisjointSets ds(n);
    vector<Edge> edges = _edges;
    sort(edges.begin(), edges.end());

    int sum = 0;
    int count = 0;
    for(const auto &e: edges) {
        if(!ds.IsSame(e.a, e.b)) {
            ds.Unite(e.a, e.b);
            sum += e.d;
            count++;
            if(count == n - 1)
                break;
        }
    }
    return sum;
}

int main() {
    int n = 6;
    vector<Edge> edges{
        {1, 2, 6},
        {1, 3, 1},
        {1, 4, 5},
        {2, 3, 5},
        {2, 5, 3},
        {3, 4, 5},
        {3, 5, 6},
        {3, 6, 4},
        {4, 6, 2},
        {5, 6, 6},
    };

    assert(Kruskal(n, edges) == 15);
}
