// SPFA (Shortest Path Faster Algorithm)
// Bellman–Ford with queue

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

#include "Graph.hpp"

constexpr int INF = 1000000000;

// O(VE)
bool SPFA(const Graph &graph, int start, vector<int> &dist) {
    dist = vector<int>(graph.size() + 1, INF);
    dist[start] = 0;
    queue<int> q;
    q.push(start);
    vector<bool> in_queue(graph.size() + 1);
    in_queue[start] = true;
    vector<int> in_count(graph.size() + 1);
    in_queue[start] = 1;

    while(!q.empty()) {
        int k = q.front();
        q.pop();
        in_queue[k] = false;
        for(const auto &e: graph.edges(k)) {
            if(dist[e.node_idx] > dist[k] + e.dist) {
                dist[e.node_idx] = dist[k] + e.dist;
                if(!in_queue[e.node_idx]) {
                    in_count[e.node_idx]++;
                    // Detect cycles.
                    if(in_count[e.node_idx] == graph.size())
                        return true;
                    q.push(e.node_idx);
                    in_queue[e.node_idx] = true;
                }
            }
        }
    }
    return false;
}

vector<int> SPFASimple(const Graph &graph, int start) {
    vector<int> dist(graph.size() + 1, INF);
    dist[start] = 0;
    queue<int> q;
    q.push(start);

    while(!q.empty()) {
        int k = q.front();
        q.pop();
        for(const auto &e: graph.edges(k)) {
            if(dist[e.node_idx] > dist[k] + e.dist) {
                dist[e.node_idx] = dist[k] + e.dist;
                q.push(e.node_idx);
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
            {3, 4, 1},
            {4, 5, 7},
            {5, 3, 3},
        };

        {
            // Undirected.
            // No negative edge.
            Graph graph(n, false);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            {
                vector<int> dist;
                if(SPFA(graph, start, dist))
                    cout << "Negative circle detected.\n";
                else
                    for(auto d: dist)
                        cout << d << "\n";
            }

            cout << "\n";

            {
                vector<int> dist = SPFASimple(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }
        }

        cout << "\n";

        {
            // Directed.
            // No negative edge.
            Graph graph(n, true);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            {
                vector<int> dist;
                if(SPFA(graph, start, dist))
                    cout << "Negative circle detected.\n";
                else
                    for(auto d: dist)
                        cout << d << "\n";
            }

            cout << "\n";

            {
                vector<int> dist = SPFASimple(graph, start);
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
            {3, 4, -100},
            {4, 5, 7},
            {5, 3, 3},
        };

        // Directed.
        // One negative edge:
        //     3 -> 4: -100
        // One negative circle: -90
        //     3 -> 4: -100
        //     4 -> 5: 7
        //     5 -> 3: 3
        Graph graph(n, true);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        vector<int> dist;
        if(SPFA(graph, start, dist))
            cout << "Negative circle detected.\n";
        else
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
            {3, 4, -10},
            {4, 5, 7},
            {5, 3, 3},
        };

        // Directed.
        // One negative edge:
        //     3 -> 4: -10
        // One zero circle: 0
        //     3 -> 4: -10
        //     4 -> 5: 7
        //     5 -> 3: 3
        Graph graph(n, true);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        {
            vector<int> dist;
            if(SPFA(graph, start, dist))
                cout << "Negative circle detected.\n";
            else
                for(auto d: dist)
                    cout << d << "\n";
        }

        cout << "\n";

        {
            vector<int> dist = SPFASimple(graph, start);
            for(auto d: dist)
                cout << d << "\n";
        }
    }

    cout << "\n";

    {
        vector<Edge> edges{
            {1, 2, 4},
            {1, 4, 2},
            {2, 3, 4},
            {2, 4, 1},
            {3, 4, -1},
            {4, 5, 7},
            {5, 3, 3},
        };

        {
            // Directed.
            // One negative edge:
            //     3 -> 4: -1
            // One positive circle: 9
            //     3 -> 4: -1
            //     4 -> 5: 7
            //     5 -> 3: 3
            Graph graph(n, true);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            {
                vector<int> dist;
                if(SPFA(graph, start, dist))
                    cout << "Negative circle detected.\n";
                else
                    for(auto d: dist)
                        cout << d << "\n";
            }

            cout << "\n";

            {
                vector<int> dist = SPFASimple(graph, start);
                for(auto d: dist)
                    cout << d << "\n";
            }
        }

        cout << "\n";

        {
            // Undirected.
            // Two negative edges:
            //     3 -> 4: -1
            //     4 -> 3: -1
            // One negative circle: -2
            //     3 -> 4: -1
            //     4 -> 3: -1
            Graph graph(n, false);
            for(const auto &e: edges)
                graph.AddEdge(e.a, e.b, e.d);

            vector<int> dist;
            if(SPFA(graph, start, dist))
                cout << "Negative circle detected.\n";
            else
                for(auto d: dist)
                    cout << d << "\n";
        }
    }

    return 0;
}
