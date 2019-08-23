// BIT (Binary Indexed Tree)

#include <cassert>
#include <cmath>

#include <limits>
#include <vector>
using namespace std;

// Index 0 is ignored.
class BinaryIndexedTree {
    size_t size_;
    vector<int> v_;
    size_t high_;

public:
    BinaryIndexedTree(size_t n) {
        high_ = n ? 1 << (size_t)log2(n) : 0;
        v_ = vector<int>(n + 1);
    }

    // O(n) initialization
    BinaryIndexedTree(const vector<int> &v) {
        size_ = v.size() - 1;
        high_ = size_ ? 1 << (size_t)log2(size_) : 0;
        v_ = v;
        for(size_t i = 1; i <= size_; i++) {
            size_t j = i + (i & -i);
            if(j <= size_)
                v_[j] += v_[i];
        }
    }

    // O(nlogn) initialization
    // BinaryIndexedTree(const vector<int> &v) {
    //     size_ = v.size() - 1;
    //     high_ = size_ ? 1 << (size_t)log2(size_) : 0;
    //     v_ = vector<int>(size_ + 1);
    //     for(size_t i = 1; i <= size_; i++)
    //         Update(i, v[i]);
    // }

    void Update(size_t i, int d) {
        for(; i < v_.size(); i += i & -i)
            v_[i] += d;
    }

    int PrefixSum(size_t i) {
        int result = 0;
        for(; i >= 1; i -= i & -i)
            result += v_[i];
        return result;
    }

    int RangeSum(size_t from, size_t to) {
        return PrefixSum(to) - PrefixSum(from - 1);
    }

    // Find the maximum index i such that PrefixSum(i) == s.
    // Return 0 if no such PrefixSum exists.
    int FindPrefixSumMax(int s) {
        size_t i = 0;
        for(size_t t = high_; t; t >>= 1) {
            size_t j = i + t;
            if(j < v_.size() && s >= v_[j]) {
                s -= v_[j];
                i = j;
            }
        }
        if(s)
            return 0;
        else
            return static_cast<int>(i);
    }

    // Find the minimum index i such that PrefixSum(i) == s.
    // Return 0 if no such PrefixSum exists.
    int FindPrefixSumMin(int S) {
        size_t i = 0;
        int s = S - 1;
        for(size_t t = high_; t; t >>= 1) {
            size_t j = i + t;
            if(j < v_.size() && s >= v_[j]) {
                s -= v_[j];
                i = j;
            }
        }
        int j = FindPrefixSumMax(S);
        if(j == 0)
            return 0;
        else
            return static_cast<int>(i + 1);
    }
};

int main() {
    // Index 0 is ignored.
    //            0  1  2  3  4  5
    vector<int> v{0, 0, 1, 2, 3, 4};
    BinaryIndexedTree bit(v);

    assert(bit.PrefixSum(1) == 0);
    assert(bit.PrefixSum(2) == 1);
    assert(bit.PrefixSum(3) == 3);
    assert(bit.PrefixSum(4) == 6);
    assert(bit.PrefixSum(5) == 10);

    assert(bit.RangeSum(1, 1) == 0);
    assert(bit.RangeSum(1, 2) == 1);
    assert(bit.RangeSum(1, 3) == 3);
    assert(bit.RangeSum(1, 4) == 6);
    assert(bit.RangeSum(1, 5) == 10);
    assert(bit.RangeSum(2, 2) == 1);
    assert(bit.RangeSum(2, 3) == 3);
    assert(bit.RangeSum(2, 4) == 6);
    assert(bit.RangeSum(2, 5) == 10);
    assert(bit.RangeSum(3, 3) == 2);
    assert(bit.RangeSum(3, 4) == 5);
    assert(bit.RangeSum(3, 5) == 9);
    assert(bit.RangeSum(4, 4) == 3);
    assert(bit.RangeSum(4, 5) == 7);
    assert(bit.RangeSum(5, 5) == 4);

    assert(bit.FindPrefixSumMax(0) == 1);
    assert(bit.FindPrefixSumMax(1) == 2);
    assert(bit.FindPrefixSumMax(3) == 3);
    assert(bit.FindPrefixSumMax(6) == 4);
    assert(bit.FindPrefixSumMax(10) == 5);

    assert(bit.FindPrefixSumMin(0) == 1);
    assert(bit.FindPrefixSumMin(1) == 2);
    assert(bit.FindPrefixSumMin(3) == 3);
    assert(bit.FindPrefixSumMin(6) == 4);
    assert(bit.FindPrefixSumMin(10) == 5);

    assert(bit.FindPrefixSumMax(-1) == 0);
    assert(bit.FindPrefixSumMax(5) == 0);
    assert(bit.FindPrefixSumMax(11) == 0);

    assert(bit.FindPrefixSumMin(-1) == 0);
    assert(bit.FindPrefixSumMin(5) == 0);
    assert(bit.FindPrefixSumMin(11) == 0);

    bit.Update(2, -1);
    assert(bit.FindPrefixSumMax(0) == 2);
    assert(bit.FindPrefixSumMax(2) == 3);
    assert(bit.FindPrefixSumMin(0) == 1);
    assert(bit.FindPrefixSumMin(2) == 3);
    bit.Update(4, -3);
    assert(bit.FindPrefixSumMax(2) == 4);
    assert(bit.FindPrefixSumMax(6) == 5);
    assert(bit.FindPrefixSumMin(2) == 3);
    assert(bit.FindPrefixSumMin(6) == 5);
    bit.Update(3, -2);
    assert(bit.FindPrefixSumMax(0) == 4);
    assert(bit.FindPrefixSumMax(4) == 5);
    assert(bit.FindPrefixSumMin(0) == 1);
    assert(bit.FindPrefixSumMin(4) == 5);

    assert(log2(0) == -numeric_limits<float>::infinity());
    assert((int)log2(0) == numeric_limits<int>::min());
    assert((int)log2(7) == 2);
    assert((int)log2(8) == 3);
    assert((int)log2(9) == 3);
}
