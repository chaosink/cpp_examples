#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <limits>

using namespace std;

class BinaryIndexedTree {
    vector<int> v_;
    size_t high_;

public:
    BinaryIndexedTree(size_t n) {
        high_ = n ? 1 << (size_t)log2(n) : 0;
        v_ = vector<int>(n + 1);
    }

    // O(n) initialization
    BinaryIndexedTree(const vector<int> &v) {
        size_t n = v.size();
        high_ = n ? 1 << (size_t)log2(n) : 0;
        n++;
        v_ = vector<int>(n);
        for(size_t i = 1; i < n; i++)
            v_[i] = v[i - 1];
        for(size_t i = 1; i < n; i++) {
            size_t j = i + (i & -i);
            if(j < n)
                v_[j] += v_[i];
        }
    }

    // O(nlogn) initialization
    // BinaryIndexedTree(const vector<int> &v) {
    //     size_t n = v.size();
    //     high_ = n ? 1 << (size_t)log2(n) : 0;
    //     n++;
    //     v_ = vector<int>(n);
    //     for(size_t i = 0; i < n - 1; i++)
    //         Update(i, v[i]);
    // }

    void Update(size_t i, int d) {
        for(++i; i < v_.size(); i += i & -i)
            v_[i] += d;
    }

    int PrefixSum(size_t i) {
        int result = 0;
        for(++i; i > 0; i -= i & -i)
            result += v_[i];
        return result;
    }

    int RangeSum(size_t from, size_t to) {
        return PrefixSum(to) - PrefixSum(from - 1);
    }

    int FindPrefixSumMax(int s) { // Find the maximum index i such that PrefixSum(i) == s.
        size_t i = 0;
        for(size_t t = high_; t; t >>= 1) {
            size_t j = i + t;
            if(j < v_.size() && s >= v_[j]) {
                s -= v_[j];
                i = j;
            }
        }
        if(s)
            return -1;
        else
            return static_cast<int>(i - 1);
    }

    int FindPrefixSumMin(int S) { // Find the minimum index i such that PrefixSum(i) == s.
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
        if(j == -1)
            return -1;
        else
            return static_cast<int>(i);
    }
};

int main() {
    vector<int> v{0, 1, 2, 3, 4};
    BinaryIndexedTree bit(v);

    assert(bit.PrefixSum(0) == 0);
    assert(bit.PrefixSum(1) == 1);
    assert(bit.PrefixSum(2) == 3);
    assert(bit.PrefixSum(3) == 6);
    assert(bit.PrefixSum(4) == 10);

    assert(bit.RangeSum(0, 0) == 0);
    assert(bit.RangeSum(0, 1) == 1);
    assert(bit.RangeSum(0, 2) == 3);
    assert(bit.RangeSum(0, 3) == 6);
    assert(bit.RangeSum(0, 4) == 10);
    assert(bit.RangeSum(1, 1) == 1);
    assert(bit.RangeSum(1, 2) == 3);
    assert(bit.RangeSum(1, 3) == 6);
    assert(bit.RangeSum(1, 4) == 10);
    assert(bit.RangeSum(2, 2) == 2);
    assert(bit.RangeSum(2, 3) == 5);
    assert(bit.RangeSum(2, 4) == 9);
    assert(bit.RangeSum(3, 3) == 3);
    assert(bit.RangeSum(3, 4) == 7);
    assert(bit.RangeSum(4, 4) == 4);

    assert(bit.FindPrefixSumMax(0) == 0);
    assert(bit.FindPrefixSumMax(1) == 1);
    assert(bit.FindPrefixSumMax(3) == 2);
    assert(bit.FindPrefixSumMax(6) == 3);
    assert(bit.FindPrefixSumMax(10) == 4);

    assert(bit.FindPrefixSumMin(0) == 0);
    assert(bit.FindPrefixSumMin(1) == 1);
    assert(bit.FindPrefixSumMin(3) == 2);
    assert(bit.FindPrefixSumMin(6) == 3);
    assert(bit.FindPrefixSumMin(10) == 4);

    assert(bit.FindPrefixSumMax(-1) == -1);
    assert(bit.FindPrefixSumMax(5) == -1);
    assert(bit.FindPrefixSumMax(11) == -1);

    assert(bit.FindPrefixSumMin(-1) == -1);
    assert(bit.FindPrefixSumMin(5) == -1);
    assert(bit.FindPrefixSumMin(11) == -1);

    bit.Update(1, -1);
    assert(bit.FindPrefixSumMax(0) == 1);
    assert(bit.FindPrefixSumMax(2) == 2);
    assert(bit.FindPrefixSumMin(0) == 0);
    assert(bit.FindPrefixSumMin(2) == 2);
    bit.Update(3, -3);
    assert(bit.FindPrefixSumMax(2) == 3);
    assert(bit.FindPrefixSumMax(6) == 4);
    assert(bit.FindPrefixSumMin(2) == 2);
    assert(bit.FindPrefixSumMin(6) == 4);
    bit.Update(2, -2);
    assert(bit.FindPrefixSumMax(0) == 3);
    assert(bit.FindPrefixSumMax(4) == 4);
    assert(bit.FindPrefixSumMin(0) == 0);
    assert(bit.FindPrefixSumMin(4) == 4);

    assert(log2(0) == -numeric_limits<float>::infinity());
    assert((int)log2(0) == numeric_limits<int>::min());
    assert((int)log2(7) == 2);
    assert((int)log2(8) == 3);
    assert((int)log2(9) == 3);
}
