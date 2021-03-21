#include <vector>
using namespace std;

// `node_idx` can be [0, n-1] or [1, n].
// A Rela represents distance `dist` to node `node_idx`.
struct Rela {
    int node_idx, dist;
    Rela(int num, int dist): node_idx(num), dist(dist) {}
    bool operator<(const Rela &rela) const {
        return dist > rela.dist;
    }
};

class Graph {
    int size_;
    bool directed_;
    // Node `i` is linked to node `edges_[i][j].node_idx` with j-th edge and distance `edges_[i][j].dist`.
    vector<vector<Rela>> edges_; // Adjacency List.

public:
    Graph(int size, bool directed = false): size_(size), directed_(directed) {
        edges_.resize(size_ + 1);
    }
    void AddEdge(int a, int b, int d) {
        edges_[a].emplace_back(b, d);
        if(!directed_)
            edges_[b].emplace_back(a, d);
    }

    int size() const {
        return size_;
    }
    const vector<Rela> &edges(int k) const {
        return edges_[k];
    }
};
