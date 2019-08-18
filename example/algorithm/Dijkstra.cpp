#include <queue>
#include <vector>
#include <iostream>
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

// O(VlogV + E)
vector<int> Dijkstra(const Graph &graph, int start) {
    vector<int> dist(graph.size() + 1, INF);
    dist[start] = 0;
    priority_queue<Node> pq;
    pq.push({start, 0});
    vector<bool> visited(graph.size() + 1);

    while(pq.size()) {
        int k = pq.top().num;
        pq.pop();
        if(visited[k])
            continue;
        for(const auto &node: graph.edges(k)) {
            if(!visited[node.num] && dist[node.num] > dist[k] + node.dist) {
                dist[node.num] = dist[k] + node.dist;
                pq.push({node.num, dist[node.num]});
            }
        }
        visited[k] = true;
    }
    return dist;
}

vector<int> DijkstraSimple(const Graph &graph, int start) {
    vector<int> dist(graph.size() + 1, INF);
    dist[start] = 0;
    priority_queue<Node> pq;
    pq.push({start, 0});

    while(pq.size()) {
        int k = pq.top().num;
        pq.pop();
        for(const auto &node: graph.edges(k)) {
            if(dist[node.num] > dist[k] + node.dist) {
                dist[node.num] = dist[k] + node.dist;
                pq.push({node.num, dist[node.num]});
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
    vector<Edge> edges{
        {1, 2, 4},
        {1, 4, 2},
        {2, 3, 4},
        {2, 4, 1},
        {3, 4, 1},
        {3, 5, 3},
        {4, 5, 7},
    };

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

    return 0;
}
