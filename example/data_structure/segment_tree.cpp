// Segment Tree
// A data structure solving the Range Information Query problem.

#include <cassert>

#include <iostream>
#include <vector>
using namespace std;

struct Info {
    using DataType = int;

    DataType sum, mi, ma;

    Info() = default;
    Info(int sum, int mi, int ma) : sum(sum), mi(mi), ma(ma) {}
    explicit Info(int val) : sum(val), mi(val), ma(val) {}
    Info(const Info &a, const Info &b) {
        sum = a.sum + b.sum;
        mi = min(a.mi, b.mi);
        ma = max(a.ma, b.ma);
    }

    bool operator==(const Info &info) const {
        return sum == info.sum && mi == info.mi && ma == info.ma;
    }
};
ostream &operator<<(ostream &os, const Info &info) {
    os << info.sum << " " << info.mi << " " << info.ma;
    return os;
}

template<typename T>
class SegmentTree {
    int size_;
    vector<T> tree_;

    void Build(const vector<typename T::DataType> &data, int l, int r, int p) {
        if(l == r) {
            tree_[p] = T{data[l]};
            return;
        }
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        Build(data, l, m, l_sub);
        Build(data, m + 1, r, r_sub);
        tree_[p] = T(tree_[l_sub], tree_[r_sub]);
    }

    void Update(int k, int val, int l, int r, int p) {
        assert(0 < k && k <= size_);
        if(l == r) {
            tree_[p] = T{val};
            return;
        }
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        if(k <= m)
            Update(k, val, l, m, l_sub);
        else
            Update(k, val, m + 1, r, r_sub);
        tree_[p] = T(tree_[l_sub], tree_[r_sub]);
    }

    T Query(int L, int R, int l, int r, int p) {
        assert(L <= R);
        if(L <= l && r <= R)
            return tree_[p];
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        if(R <= m)
            return Query(L, R, l, m, l_sub);
        if(L > m)
            return Query(L, R, m + 1, r, r_sub);
        return T(Query(L, R, l, m, l_sub), Query(L, R, m + 1, r, r_sub));
    }

public:
    SegmentTree(int size) {
        size_ = size;
        tree_.resize(size_ * 4 + 1);
    }

    // Indexes of `data` start from 1.
    SegmentTree(const vector<typename T::DataType> &data) {
        size_ = data.size() - 1;
        tree_.resize(size_ * 4 + 1);

        // O(nlogn) initialization
        // for(int i = 1; i <= size_; i++)
        //     Update(i, data[i], 1, size_, 1);

        // O(logn) initialization
        Build(data, 1, size_, 1);
    }

    void Update(int k, int val) {
        assert(0 < k && k <= size_);
        Update(k, val, 1, size_, 1);
    }

    T Query(int L, int R) {
        assert(L <= R);
        return Query(L, R, 1, size_, 1);
    }
};

// Query the maximum of a range.
class SegmentTreeSimple {
    int size_;
    vector<int> tree_;

    void Build(const vector<int> &data, int l, int r, int p) {
        if(l == r) {
            tree_[p] = data[l];
            return;
        }
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        Build(data, l, m, l_sub);
        Build(data, m + 1, r, r_sub);
        tree_[p] = max(tree_[l_sub], tree_[r_sub]);
    }

    void Update(int k, int val, int l, int r, int p) {
        assert(0 < k && k <= size_);
        if(l == r) {
            tree_[p] = val;
            return;
        }
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        if(k <= m)
            Update(k, val, l, m, l_sub);
        else
            Update(k, val, m + 1, r, r_sub);
        tree_[p] = max(tree_[l_sub], tree_[r_sub]);
    }

    int Query(int L, int R, int l, int r, int p) {
        assert(L <= R);
        if(L <= l && r <= R)
            return tree_[p];
        int m = (l + r) >> 1;
        int l_sub = p << 1, r_sub = l_sub | 1;
        if(R <= m)
            return Query(L, R, l, m, l_sub);
        if(L > m)
            return Query(L, R, m + 1, r, r_sub);
        return max(Query(L, R, l, m, l_sub), Query(L, R, m + 1, r, r_sub));
    }

public:
    SegmentTreeSimple(int size) {
        size_ = size;
        tree_.resize(size_ * 4 + 1);
    }

    // Indexes of `data` start from 1.
    SegmentTreeSimple(const vector<int> &data) {
        size_ = data.size() - 1;
        tree_.resize(size_ * 4 + 1);

        // O(nlogn) initialization
        // for(int i = 1; i <= size_; i++)
        //     Update(i, data[i], 1, size_, 1);

        // O(logn) initialization
        Build(data, 1, size_, 1);
    }

    void Update(int k, int val) {
        assert(0 < k && k <= size_);
        Update(k, val, 1, size_, 1);
    }

    int Query(int L, int R) {
        assert(L <= R);
        return Query(L, R, 1, size_, 1);
    }
};

int main() {
    // clang-format off
    vector<int> data { // Index 0 is ignored.
        0, 1, 9, 2, 5, 3, 8, 6, 4, 7,
    };
    // clang-format on

    {
        SegmentTree<Info> st(data);

        // clang-format off
        assert(st.Query(0, 9) == Info(45, 1, 9)); // OK
        assert(st.Query(1,10) == Info(45, 1, 9)); // OK
        // st.Query(9, 1); // error

        //                           sum min max
        assert(st.Query(1, 1) == Info( 1, 1, 1));
        assert(st.Query(1, 2) == Info(10, 1, 9));
        assert(st.Query(1, 3) == Info(12, 1, 9));
        assert(st.Query(1, 4) == Info(17, 1, 9));
        assert(st.Query(1, 5) == Info(20, 1, 9));
        assert(st.Query(1, 6) == Info(28, 1, 9));
        assert(st.Query(1, 7) == Info(34, 1, 9));
        assert(st.Query(1, 8) == Info(38, 1, 9));
        assert(st.Query(1, 9) == Info(45, 1, 9));

        assert(st.Query(3, 3) == Info( 2, 2, 2));
        assert(st.Query(3, 4) == Info( 7, 2, 5));
        assert(st.Query(3, 5) == Info(10, 2, 5));
        assert(st.Query(3, 6) == Info(18, 2, 8));
        assert(st.Query(3, 7) == Info(24, 2, 8));
        assert(st.Query(3, 8) == Info(28, 2, 8));
        assert(st.Query(3, 9) == Info(35, 2, 8));

        assert(st.Query(5, 5) == Info( 3, 3, 3));
        assert(st.Query(5, 6) == Info(11, 3, 8));
        assert(st.Query(5, 7) == Info(17, 3, 8));
        assert(st.Query(5, 8) == Info(21, 3, 8));
        assert(st.Query(5, 9) == Info(28, 3, 8));

        assert(st.Query(7, 7) == Info( 6, 6, 6));
        assert(st.Query(7, 8) == Info(10, 4, 6));
        assert(st.Query(7, 9) == Info(17, 4, 7));

        assert(st.Query(9, 9) == Info( 7, 7, 7));
        // clang-format on
    }

    {
        SegmentTreeSimple st(data);

        // clang-format off
        assert(st.Query(0, 9) == 9); // OK
        assert(st.Query(1,10) == 9); // OK
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
        // clang-format on
    }

    return 0;
}
