// Prim
// A minimum-spanning-tree algorithm.

#include <cassert>

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#include "Graph.hpp"

int Prim(const Graph &graph) {
    priority_queue<Rela> pq;
    pq.push({1, 0});
    vector<bool> in_tree(graph.size() + 1);

    int sum = 0;
    while(!pq.empty()) {
        int k = pq.top().node_idx;
        if(!in_tree[k]) {
            sum += pq.top().dist;
            in_tree[k] = true;
        }
        pq.pop();
        for(const auto &e: graph.edges(k))
            if(!in_tree[e.node_idx])
                pq.push(e);
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
