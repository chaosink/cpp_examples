#include <cassert>
#include <ctime>

#include <algorithm>
#include <functional>
#include <queue>
#include <vector>
#include <numeric>
#include <random>
#include <set>

template<class T,
         class Container = std::vector<T>,
         class Compare = std::less<typename Container::value_type>>
class priority_queue {
    inline size_t Left(size_t k) {
        size_t left = k * 2 + 1;
        if(left < container_.size())
            return left;
        return k;
    }
    inline size_t Right(size_t k) {
        size_t right = k * 2 + 2;
        if(right < container_.size())
            return right;
        return k;
    }
    inline size_t Parent(size_t k) {
        return (k - 1) / 2;
    }
    void Up() {
        size_t k = container_.size() - 1;
        size_t parent = Parent(k);

        while(k && compare_(container_[parent], container_[k])) {
            std::swap(container_[k], container_[parent]);
            k = parent;
            parent = Parent(k);
        }
    }
    void Down() {
        if(container_.size() == 0)
            return;

        size_t k = 0;
        size_t left = Left(k);
        size_t right = Right(k);

        while(compare_(container_[k], container_[left])
              || compare_(container_[k], container_[right])) {
            if(compare_(container_[right], container_[left])) {
                std::swap(container_[k], container_[left]);
                k = left;
            } else {
                std::swap(container_[k], container_[right]);
                k = right;
            }
            left = Left(k);
            right = Right(k);
        }
    }

protected:
    Container container_;
    Compare compare_;

public:
    priority_queue(const Compare &compare = Compare()): compare_(compare) {}

    void push(const T &value) {
        container_.push_back(value);
        Up();
    }

    void push(T &&value) {
        container_.push_back(value);
        Up();
    }

    void pop() {
        container_.front() = container_.back();
        container_.pop_back();
        Down();
    }

    const T &top() const {
        return container_.front();
    }

    size_t size() const {
        return container_.size();
    }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int k = 100;

    // Random numbers with posibility of repetition. Max first.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<int> v(n);
        for(int i = 0; i < n; i++)
            v[i] = std::rand() % n;

        priority_queue<int> pq;
        std::priority_queue<int> std_pq;

        for(int i = 0; i < n; i++) {
            pq.push(v[i]);
            std_pq.push(v[i]);
        }

        for(int i = 0; i < n; i++) {
            assert(pq.top() == std_pq.top());
            pq.pop();
            std_pq.pop();
        }
    }

    // Random numbers with posibility of repetition. Min first.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<int> v(n);
        for(int i = 0; i < n; i++)
            v[i] = std::rand() % n;

        priority_queue<int, std::vector<int>, std::greater<int>> pq;
        std::priority_queue<int, std::vector<int>, std::greater<int>> std_pq;

        for(int i = 0; i < n; i++) {
            pq.push(v[i]);
            std_pq.push(v[i]);
        }

        for(int i = 0; i < n; i++) {
            assert(pq.top() == std_pq.top());
            pq.pop();
            std_pq.pop();
        }
    }

    // Random numbers without posibility of repetition. Max first. `std::set` usable also.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<int> v(n);
        std::iota(v.begin(), v.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(v.begin(), v.end(), g);

        priority_queue<int> pq;
        std::priority_queue<int> std_pq;
        std::set<int, std::greater<int>> std_set;

        for(int i = 0; i < n; i++) {
            pq.push(v[i]);
            std_pq.push(v[i]);
            std_set.insert(v[i]);
        }

        for(int i = 0; i < n; i++) {
            assert(pq.top() == std_pq.top());
            assert(pq.top() == *std_set.begin());
            pq.pop();
            std_pq.pop();
            std_set.erase(std_set.begin());
        }
    }

    // Random numbers without posibility of repetition. Min first. `std::set` usable also.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<int> v(n);
        std::iota(v.begin(), v.end(), 0);
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(v.begin(), v.end(), g);

        priority_queue<int, std::vector<int>, std::greater<int>> pq;
        std::priority_queue<int, std::vector<int>, std::greater<int>> std_pq;
        std::set<int> std_set;

        for(int i = 0; i < n; i++) {
            pq.push(v[i]);
            std_pq.push(v[i]);
            std_set.insert(v[i]);
        }

        for(int i = 0; i < n; i++) {
            assert(pq.top() == std_pq.top());
            assert(pq.top() == *std_set.begin());
            pq.pop();
            std_pq.pop();
            std_set.erase(std_set.begin());
        }
    }
}
