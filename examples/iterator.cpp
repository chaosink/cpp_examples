#include "common.hpp"

struct S {
	int size = 5;
	vector<int> data;

	S() {
		for(int i = 0; i < size; ++i)
			data.push_back(i);
	}

	struct It {
		vector<int> &data;
		int i;

		It(vector<int> &data, int i): data(data), i(i) {}
		It& operator++() {
			++i;
			return *this;
		}
		bool operator!=(const It &other) {
			return i != other.i;;
		}
		int operator*() {
			return data[i];
		}
	};

	It begin() {
		return It(data, 0);
	}

	It end() {
		return It(data, size);
	}
};

int main() {
	S s;
	for(auto i: s)
		cout << i << endl;
}
