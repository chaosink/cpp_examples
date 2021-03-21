// Dijkstra with priority queue

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#include "Graph.hpp"

constexpr int INF = 1000000000;

// O(VlogV + E)
// 对于有向或无向图，有负权边也可以停止，但结果可能不对。
vector<int> Dijkstra(const Graph &graph, int start) {
    vector<int> dist(graph.size() + 1, INF);
    dist[start] = 0;
    priority_queue<Rela> pq;
    pq.push({start, 0});
    vector<bool> visited(graph.size() + 1);

    while(!pq.empty()) {
        int k = pq.top().node_idx;
        pq.pop();
        if(visited[k])
            continue;
        for(const auto &e: graph.edges(k)) {
            if(!visited[e.node_idx] && dist[e.node_idx] > dist[k] + e.dist) {
                dist[e.node_idx] = dist[k] + e.dist;
                pq.push({e.node_idx, dist[e.node_idx]});
            }
        }
        visited[k] = true;
    }
    return dist;
}

// 有负环(有向图的负环，无向图的负边即负环)就不会停止，否则(零环、正环)可以得到正确结果。
// 将priority_queue改为queue就和`SPFASimple()`完全一样了。
//      所以可以处理有负权边但无负环的图。`SPFASimple()`就可以。
//      两者的计算结果完全一样，priority_queue可以节省常数时间。
//      相比于`Dijkstra()`，省去了集合操作。
//          因为假设没有负权边。
vector<int> DijkstraSimple(const Graph &graph, int start) {
    vector<int> dist(graph.size() + 1, INF);
    dist[start] = 0;
    priority_queue<Rela> pq;
    pq.push({start, 0});

    while(!pq.empty()) {
        int k = pq.top().node_idx;
        pq.pop();
        for(const auto &e: graph.edges(k)) {
            if(dist[e.node_idx] > dist[k] + e.dist) {
                dist[e.node_idx] = dist[k] + e.dist;
                pq.push({e.node_idx, dist[e.node_idx]});
            }
        }
    }
    return dist;
}

int main() {
    int n = 5;
    int start = 1;
    struct Edge {
        int a, b, d;
    };

    {
        vector<Edge> edges{
            {1, 2, 4},
            {1, 4, 2},
            {2, 3, 4},
            {2, 4, 1},
            // 如果将边3-4的权值改为-100，
            //      对于无向图，处处有环，故有负边即有负环，
            //          `Dijkstra()`会得到错误结果，
            //          `DijkstraSimple()`不会停止。(负环)
            //      对于有向图，节点3，4，5构成一个环，且为负环，
            //          `Dijkstra()`会得到错误结果，
            //          `DijkstraSimple()`不会停止。(负环)
            // 如果将边3-4的权值改为-10，
            //      对于无向图，处处有环，故有负边即有负环，
            //          `Dijkstra()`会得到错误结果，
            //          `DijkstraSimple()`不会停止。(负环)
            //      对于有向图，节点3，4，5构成一个环，且为零环，
            //          `Dijkstra()`会得到错误结果，
            //          `DijkstraSimple()`会得到正确结果。(无负环)
            // 如果将边3-4的权值改为-1，
            //      对于无向图，处处有环，故有负边即有负环，
            //          `Dijkstra()`会得到错误的结果，
            //          `DijkstraSimple()`不会停止。(负环)
            //      对于有向图，节点3，4，5构成一个环，且为正环，
            //          `Dijkstra()`会得到错误的结果，
            //          `DijkstraSimple()`会得到正确结果。(无负环)
            // 只要没有负环，`DijkstraSimple()`就可以得到正确结果。
            //      `DijkstraSimple()`类似`SPFASimple()`。
            {3, 4, 1},
            {4, 5, 7},
            {5, 3, 3},
        };

        {
            Graph graph(n, false);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            {
                vector<int> dist = Dijkstra(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }

            cout << "\n";

            {
                vector<int> dist = DijkstraSimple(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }
        }

        cout << "\n";

        {
            Graph graph(n, true);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            {
                vector<int> dist = Dijkstra(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }

            cout << "\n";

            {
                vector<int> dist = DijkstraSimple(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }
        }
    }

    cout << "\n";

    {
        vector<Edge> edges{
            {1, 2, 4},
            {1, 4, 2},
            {2, 3, 4},
            {2, 4, 1},
            {3, 4, -10}, // 零环
            {4, 5, 7},
            {5, 3, 3},
        };

        Graph graph(n, true);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        vector<int> dist = DijkstraSimple(graph, start);
        for(auto d: dist)
            cout << d << "\n";
    }

    cout << "\n";

    {
        vector<Edge> edges{
            {1, 2, 4},
            {1, 4, 2},
            {2, 3, 4},
            {2, 4, 1},
            {3, 4, -1}, // 正环
            {4, 5, 7},
            {5, 3, 3},
        };

        Graph graph(n, true);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        vector<int> dist = DijkstraSimple(graph, start);
        for(auto d: dist)
            cout << d << "\n";
    }

    return 0;
}
