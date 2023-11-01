#include <cassert>
#include <ctime>

#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <random>
#include <set>
#include <vector>

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
    void Up(size_t k) {
        size_t parent = Parent(k);

        while(k && compare_(container_[parent], container_[k])) {
            std::swap(container_[k], container_[parent]);
            std::swap(*handles_[k].value_id_, *handles_[parent].value_id_);
            std::swap(handles_[k].value_id_, handles_[parent].value_id_);
            k = parent;
            parent = Parent(k);
        }
    }
    void Down(size_t k) {
        if(container_.size() == 0)
            return;

        size_t left = Left(k);
        size_t right = Right(k);

        while(compare_(container_[k], container_[left])
              || compare_(container_[k], container_[right])) {
            if(compare_(container_[right], container_[left])) {
                std::swap(container_[k], container_[left]);
                std::swap(*handles_[k].value_id_, *handles_[left].value_id_);
                std::swap(handles_[k].value_id_, handles_[left].value_id_);
                k = left;
            } else {
                std::swap(container_[k], container_[right]);
                std::swap(*handles_[k].value_id_, *handles_[right].value_id_);
                std::swap(handles_[k].value_id_, handles_[right].value_id_);
                k = right;
            }
            left = Left(k);
            right = Right(k);
        }
    }

public:
    class handle_type {
        std::shared_ptr<size_t> value_id_;

        handle_type(size_t value_id): value_id_(std::make_shared<size_t>(value_id)) {}

        friend class priority_queue;
    };

protected:
    Container container_;
    Compare compare_;
    std::vector<handle_type> handles_;

public:
    priority_queue(const Compare &compare = Compare()): compare_(compare) {}

    handle_type push(const T &value) {
        handles_.push_back(container_.size());
        handle_type handle = handles_.back();
        container_.push_back(value);
        Up(container_.size() - 1);
        return handle;
    }

    handle_type push(T &&value) {
        handles_.push_back(container_.size());
        handle_type handle = handles_.back();
        container_.push_back(value);
        Up(container_.size() - 1);
        return handle;
    }

    void pop() {
        container_.front() = container_.back();
        container_.pop_back();
        // 2 interesting assignments without using `swap()`!
        *handles_.back().value_id_ = *handles_.front().value_id_;
        handles_.front().value_id_ = handles_.back().value_id_;
        handles_.pop_back();
        Down(0);
    }

    void update(const handle_type &handle, const T &value) {
        size_t value_id = *handle.value_id_;
        bool down = compare_(value, container_[value_id]);
        container_[value_id] = value;
        if(down)
            Down(value_id);
        else
            Up(value_id);
    }

    const T &top() const {
        return container_.front();
    }

    const T &get(const handle_type &handle) const {
        return container_[*handle.value_id_];
    }

    size_t size() const {
        return container_.size();
    }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int k = 100;

    using value_type = std::pair<int, int>;

    // Random pairs with `second` as unique indexes. Top is max.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<value_type> v(n);
        for(int i = 0; i < n; i++)
            v[i] = {std::rand() % n, i};

        priority_queue<value_type> pq;
        std::set<value_type, std::greater<value_type>> std_set;

        std::vector<decltype(pq)::handle_type> handles;
        for(int i = 0; i < n; i++) {
            decltype(pq)::handle_type handle = pq.push(v[i]);
            assert(pq.get(handle) == v[i]);
            handles.push_back(handle);
            std_set.insert(v[i]);
            assert(pq.top() == *std_set.begin());
        }

        for(int i = 0; i < n; i++) {
            std_set.erase(std_set.find(v[i]));
            v[i].first = std::rand() % n;
            pq.update(handles[i], v[i]);
            std_set.insert(v[i]);
            assert(pq.get(handles[i]) == v[i]);
            assert(pq.top() == *std_set.begin());
        }

        for(int i = 0; i < n; i++) {
            value_type value = pq.top();
            assert(pq.get(handles[value.second]) == value);
            assert(pq.top() == *std_set.begin());
            pq.pop();
            std_set.erase(std_set.begin());
        }
    }

    // Random pairs with `second` as unique indexes. Top is max.
    for(int i = 0; i < k; i++) {
        int n = std::rand() % 10000;

        std::vector<value_type> v(n);
        for(int i = 0; i < n; i++)
            v[i] = {std::rand() % n, i};

        priority_queue<value_type, std::vector<value_type>, std::greater<value_type>> pq;
        std::set<value_type> std_set;

        std::vector<decltype(pq)::handle_type> handles;
        for(int i = 0; i < n; i++) {
            decltype(pq)::handle_type handle = pq.push(v[i]);
            assert(pq.get(handle) == v[i]);
            handles.push_back(handle);
            std_set.insert(v[i]);
            assert(pq.top() == *std_set.begin());
        }

        for(int i = 0; i < n; i++) {
            std_set.erase(std_set.find(v[i]));
            v[i].first = std::rand() % n;
            pq.update(handles[i], v[i]);
            std_set.insert(v[i]);
            assert(pq.get(handles[i]) == v[i]);
            assert(pq.top() == *std_set.begin());
        }

        for(int i = 0; i < n; i++) {
            value_type value = pq.top();
            assert(pq.get(handles[value.second]) == value);
            assert(pq.top() == *std_set.begin());
            pq.pop();
            std_set.erase(std_set.begin());
        }
    }
}
