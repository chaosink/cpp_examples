#include <iostream>
#include <queue>
#include <vector>
using namespace std;

constexpr int row = 15, col = 20;
// clang-format off
constexpr int map[row][col] = { // `1` is obstacle.
    {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,1},
    {0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1},
    {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,0,1},
    {0,0,1,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
};
constexpr int step[4][2] = {
    {-1, 0},
    { 1, 0},
    { 0,-1},
    { 0, 1},
};
// clang-format on

struct Node {
    int x, y, h0, h1;
    bool operator<(const Node &node) const {
        return h0 + h1 > node.h0 + node.h1;
    }
};

bool Valid(int x, int y) {
    return x >= 0 && x < row && y >= 0 && y < col;
}

int Heuristic(int x, int y, int x1, int y1) {
    return std::abs(x - x1) + std::abs(y - y1);
}

bool AStar(int x0, int y0, int x1, int y1, int &dist) {
    if(map[x0][y0] || map[x1][y1])
        return false;

    priority_queue<Node> pq;
    pq.push({x0, y0, 0, Heuristic(x0, y0, x1, y1)});
    vector<vector<int>> visited(row, vector<int>(col));
    visited[x0][y0] = true;

    while(!pq.empty()) {
        Node node = pq.top();
        if(node.x == x1 && node.y == y1) {
            dist = node.h0;
            return true;
        }
        pq.pop();
        visited[node.x][node.y] = true;
        for(int i = 0; i < 4; i++) {
            int x = node.x + step[i][0];
            int y = node.y + step[i][1];
            if(Valid(x, y) && !map[x][y] && !visited[x][y])
                pq.push({x, y, node.h0 + 1, Heuristic(x, y, x1, y1)});
        }
    }
    return false;
}

int main() {
    int dist;

    {
        int x0 = 0, y0 = 0, x1 = row - 1, y1 = col - 1;
        if(AStar(x0, y0, x1, y1, dist))
            cout << "Shortest distance: " << dist << endl;
        else
            cout << "The target is unreachable." << endl;
    }

    cout << endl;

    {
        int x0 = 0, y0 = 0, x1 = row - 1, y1 = 0;
        if(AStar(x0, y0, x1, y1, dist))
            cout << "Shortest distance: " << dist << endl;
        else
            cout << "No path exists." << endl;
    }

    return 0;
}
