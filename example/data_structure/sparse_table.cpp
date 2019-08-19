// ST (Sparse Table)
// A data structure solving the RMQ (Range Minimum / Maximum Query) problem.

#include <cassert>

#include <vector>
using namespace std;

class SparseTable {
    int size_;
    vector<int> log2_;
    vector<int> exp2_;
    vector<vector<int>> dp_;

public:
    // Indexes of `data` start from 1.
    SparseTable(const vector<int> &data) {
        size_ = data.size() - 1;

        log2_.resize(size_ + 1);
        for(int i = 2; i <= size_; i++)
            log2_[i] = log2_[i >> 1] + 1; // Pre-compute table of `log2()`.

        exp2_.resize(log2_[size_] + 1);
        exp2_[0] = 1;
        for(int i = 1; i <= log2_[size_]; i++)
            exp2_[i] = exp2_[i - 1] * 2; // Pre-compute table of `exp2()`.

        dp_.resize(log2_[size_] + 1, vector<int>(size_ + 1));
        for(int i = 1; i <= size_; i++)
            dp_[0][i] = data[i];
        for(int j = 1; j <= log2_[size_]; j++)
            for(int i = 1; i + exp2_[j] - 1 <= size_; i++)
                dp_[j][i] = max(dp_[j - 1][i], dp_[j - 1][i + exp2_[j - 1]]);
    }

    int Query(int l, int r) {
        assert(l <= r && l > 0 && r <= size_);
        int e = log2_[r - l + 1];
        return max(dp_[e][l], dp_[e][r - exp2_[e] + 1]);
    }
};

int main() {
    // clang-format off
    vector<int> data { // Index 0 is ignored.
        0, 1, 9, 2, 5, 3, 8, 6, 4, 7,
    };
    // clang-format on

    SparseTable st(data);

    // st.Query(0, 9); // error
    // st.Query(1,10); // error
    // st.Query(9, 1); // error

    assert(st.Query(1, 1) == 1);
    assert(st.Query(1, 2) == 9);
    assert(st.Query(1, 3) == 9);
    assert(st.Query(1, 4) == 9);
    assert(st.Query(1, 5) == 9);
    assert(st.Query(1, 6) == 9);
    assert(st.Query(1, 7) == 9);
    assert(st.Query(1, 8) == 9);
    assert(st.Query(1, 9) == 9);

    assert(st.Query(3, 3) == 2);
    assert(st.Query(3, 4) == 5);
    assert(st.Query(3, 5) == 5);
    assert(st.Query(3, 6) == 8);
    assert(st.Query(3, 7) == 8);
    assert(st.Query(3, 8) == 8);
    assert(st.Query(3, 9) == 8);

    assert(st.Query(5, 5) == 3);
    assert(st.Query(5, 6) == 8);
    assert(st.Query(5, 7) == 8);
    assert(st.Query(5, 8) == 8);
    assert(st.Query(5, 9) == 8);

    assert(st.Query(7, 7) == 6);
    assert(st.Query(7, 8) == 6);
    assert(st.Query(7, 9) == 7);

    assert(st.Query(9, 9) == 7);

    return 0;
}
