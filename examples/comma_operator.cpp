#include "common.hpp"

struct S {
	struct SS {
		vector<int> data;

		SS& operator,(int i) {
			data.push_back(i);
			return *this;
		}
	};
	SS ss;

	SS& operator<<(int i) {
		return ss, i;
	}

	auto begin() {
		return ss.data.begin();
	}
	auto end() {
		return ss.data.end();
	}
};

int main() {
	S s;
	s << 0, 1, 2, 3, 4;
	for(auto i: s)
		cout << i << endl;
}
