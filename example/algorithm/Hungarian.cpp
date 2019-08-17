#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Hungarian {
    int n_;
    bool floyd_;
    vector<vector<bool>> connected_;
    vector<bool> visited_;
    vector<int> matched_from_;

    void Floyd() {
        for(int k = 1; k <= n_; k++)
            for(int i = 1; i <= n_; i++)
                for(int j = 1; j <= n_; j++)
                    if(connected_[i][k] && connected_[k][j])
                        connected_[i][j] = true;
    }

    bool DFS(int k) {
        for(int i = 1; i <= n_; i++)
            if(!visited_[i] && connected_[k][i]) {
                visited_[i] = true;
                if(matched_from_[i] == -1 || DFS(matched_from_[i])) {
                    matched_from_[i] = k;
                    return true;
                }
            }
        return false;
    }

public:
    // Indexes can be [0, n-1] or [1, n].
    // connections: int connections[connection_count][2];
    // floyd: extend connections with Floyd.
    void Build(int n, const vector<vector<int>> &connections, bool floyd = false) {
        n_ = n;
        floyd_ = floyd;
        connected_.clear();
        connected_.resize(n_ + 1, vector<bool>(n_ + 1));
        for(const auto &c: connections)
            connected_[c[0]][c[1]] = true;
        if(floyd_)
            Floyd();

        visited_.resize(n_ + 1);
        matched_from_.resize(n_ + 1);
    }

    int Match() {
        fill(matched_from_.begin(), matched_from_.end(), -1);
        int match_count = 0;
        for(int i = 1; i <= n_; i++) {
            fill(visited_.begin(), visited_.end(), false);
            if(DFS(i))
                match_count++;
        }
        return match_count;
    }
};

int main() {
    int n = 5;
    vector<vector<int>> connections{
        {1, 3},
        {2, 3},
        {3, 4},
        {3, 5},
    };

    Hungarian hungarian;

    // Don't extend connections with Floyd.
    hungarian.Build(n, connections, false);
    cout << hungarian.Match() << "\n"; // 2

    // Extend connections with Floyd.
    hungarian.Build(n, connections, true);
    cout << hungarian.Match() << "\n"; // 3

    return 0;
}
