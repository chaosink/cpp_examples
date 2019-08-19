// SPFA (Shortest Path Faster Algorithm)
// Bellman–Ford with queue

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

constexpr int INF = 1000000000;

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
    const vector<Node> edges(int k) const {
        return edges_[k];
    }
};

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
        for(const auto &node: graph.edges(k)) {
            if(dist[node.num] > dist[k] + node.dist) {
                dist[node.num] = dist[k] + node.dist;
                if(!in_queue[node.num]) {
                    in_count[node.num]++;
                    // Detect cycles.
                    if(in_count[node.num] == graph.size())
                        return true;
                    q.push(node.num);
                    in_queue[node.num] = true;
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
        for(const auto &node: graph.edges(k)) {
            if(dist[node.num] > dist[k] + node.dist) {
                dist[node.num] = dist[k] + node.dist;
                q.push(node.num);
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
            {3, 4, -1},
            {4, 5, 7},
            {5, 3, 3},
        };

        {
            // Directed.
            // One negative edge:
            //     3 -> 4: -1
            // No genative circle.
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
            // Undirected.
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

    cout << "\n";

    {
        vector<Edge> edges{
            {1, 2, 4},
            {1, 4, 2},
            {2, 3, 4},
            {2, 4, 1},
            {3, 4, 1},
            {4, 5, -7},
            {5, 3, 3},
        };

        // Directed.
        // One negative edge:
        //     4 -> 5: -7
        // One negative circle: -3
        //     4 -> 5: -7
        //     5 -> 3: 3
        //     3 -> 4: 1
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
            {3, 4, 1},
            {4, 5, 7},
            {5, 3, 3},
        };

        Graph graph(n, false);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        vector<int> dist = SPFASimple(graph, start);
        for(auto d: dist)
            cout << d << "\n";
    }

    return 0;
}
