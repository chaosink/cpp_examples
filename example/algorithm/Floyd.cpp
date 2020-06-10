#include <iostream>
#include <queue>
#include <vector>
using namespace std;

constexpr int INF = 1000000000;

// Index `num` can be [0, n-1] or [1, n].
// A node is linked to node `num` with distance `dist`.
struct Node {
    int num, dist;
    Node(int num, int dist): num(num), dist(dist) {}
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
    const vector<Node> &edges(int k) const {
        return edges_[k];
    }
};

// O(V^3)
bool Floyd(const Graph &graph, vector<vector<int>> &dist) {
    dist = vector<vector<int>>(graph.size() + 1, vector<int>(graph.size() + 1, INF));
    for(int i = 0; i <= graph.size(); i++) {
        dist[i][i] = 0;
        for(const auto &node: graph.edges(i))
            dist[i][node.num] = node.dist;
    }

    for(int k = 0; k <= graph.size(); k++)
        for(int i = 0; i <= graph.size(); i++)
            for(int j = 0; j <= graph.size(); j++)
                if(dist[i][j] > dist[i][k] + dist[k][j])
                    dist[i][j] = dist[i][k] + dist[k][j];

    // Detect cycles.
    for(int i = 0; i <= graph.size(); i++)
        if(dist[i][i] < 0)
            return true;

    return false;
}

vector<vector<int>> FloydSimple(const Graph &graph) {
    vector<vector<int>> dist(graph.size() + 1, vector<int>(graph.size() + 1, INF));
    for(int i = 0; i <= graph.size(); i++) {
        dist[i][i] = 0;
        for(const auto &node: graph.edges(i))
            dist[i][node.num] = node.dist;
    }

    for(int k = 0; k <= graph.size(); k++)
        for(int i = 0; i <= graph.size(); i++)
            for(int j = 0; j <= graph.size(); j++)
                if(dist[i][j] > dist[i][k] + dist[k][j])
                    dist[i][j] = dist[i][k] + dist[k][j];

    return dist;
}

int main() {
    int n = 5;
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

            vector<vector<int>> dist;
            if(Floyd(graph, dist))
                cout << "Negative circle detected.\n";
            else
                for(auto dd: dist) {
                    for(auto d: dd)
                        printf("%11d", d);
                    cout << "\n";
                }
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

            vector<vector<int>> dist;
            if(Floyd(graph, dist))
                cout << "Negative circle detected.\n";
            else
                for(auto dd: dist) {
                    for(auto d: dd)
                        printf("%11d", d);
                    cout << "\n";
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

        vector<vector<int>> dist;
        if(Floyd(graph, dist))
            cout << "Negative circle detected.\n";
        else
            for(auto dd: dist) {
                for(auto d: dd)
                    printf("%11d", d);
                cout << "\n";
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
            {4, 5, 7},
            {5, 3, 3},
        };

        Graph graph(n, false);
        for(const auto &e: edges)
            graph.AddEdge(e.a, e.b, e.d);

        vector<vector<int>> dist = FloydSimple(graph);
        for(auto dd: dist) {
            for(auto d: dd)
                printf("%11d", d);
            cout << "\n";
        }
    }

    return 0;
}
