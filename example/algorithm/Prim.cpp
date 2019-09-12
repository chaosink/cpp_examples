// Prim
// A minimum-spanning-tree algorithm.

#include <cassert>

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

// Index `num` can be [0, n-1] or [1, n].
// A node is linked to node `num` with distance `dist`.
struct Node {
    int num, dist;
    Node(int num, int dist) : num(num), dist(dist) {}
    bool operator<(const Node &node) const {
        return dist > node.dist;
    }
};

class Graph {
    int size_;
    bool directed_;
    // Node `i` is linked to node `edges_[i].k` with distance `edges_[i].d`.
    vector<vector<Node>> edges_; // Adjacency List.

public:
    Graph(int size, bool directed = false) : size_(size), directed_(directed) {
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
    const vector<Node> &edges(int k) const {
        return edges_[k];
    }
};

int Prim(const Graph &graph) {
    priority_queue<Node> pq;
    pq.push({1, 0});
    vector<bool> in_tree(graph.size() + 1);

    int sum = 0;
    while(!pq.empty()) {
        int k = pq.top().num;
        if(!in_tree[k]) {
            sum += pq.top().dist;
            in_tree[k] = true;
        }
        pq.pop();
        for(const auto &node: graph.edges(k))
            if(!in_tree[node.num])
                pq.push(node);
    }
    return sum;
}

int main() {
    int n = 6;
    struct Edge {
        int a, b, d;
    };
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

    Graph graph(n, false);
    for(const auto &e: edges)
        graph.AddEdge(e.a, e.b, e.d);

    assert(Prim(graph) == 15);
}
