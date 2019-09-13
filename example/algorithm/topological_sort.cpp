#include <cassert>

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

// Node indexes start from 0.
class Graph {
    int size_;
    // Node `i` is linked to node `edges_[i][j]`.
    vector<vector<int>> edges_; // Adjacency List.

public:
    Graph(int size) : size_(size) {
        edges_.resize(size_);
    }
    void AddEdge(int a, int b) {
        edges_[a].push_back(b);
    }

    int size() const {
        return size_;
    }
    const vector<int> &edges(int k) const {
        return edges_[k];
    }
};

vector<int> TopologicalSort(const Graph &graph) {
    int n = graph.size();
    vector<int> in(n);
    for(int i = 0; i < n; i++)
        for(int k: graph.edges(i))
            in[k]++;

    queue<int> q;
    for(int i = 0; i < n; i++)
        if(in[i] == 0)
            q.push(i);

    vector<int> ans;
    while(!q.empty()) {
        int k = q.front();
        ans.push_back(k);
        q.pop();
        for(int i: graph.edges(k)) {
            in[i]--;
            if(in[i] == 0)
                q.push(i);
        }
    }

    return ans;
}

int main() {
    struct Edge {
        int a, b;
    };

    {
        int n = 6;
        vector<Edge> edges{
            {5, 2},
            {5, 0},
            {4, 0},
            {4, 1},
            {2, 3},
            {3, 1},
        };

        Graph graph(n);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b);

        vector<int> ans = TopologicalSort(graph);
        assert(ans == vector<int>({4, 5, 2, 0, 3, 1}));
    }

    {
        int n = 3;
        vector<Edge> edges{
            {0, 1},
            {1, 2},
            {2, 0},
        }; // A cycle.

        Graph graph(n);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b);

        vector<int> ans = TopologicalSort(graph);
        assert(ans.empty());
    }

    return 0;
}
