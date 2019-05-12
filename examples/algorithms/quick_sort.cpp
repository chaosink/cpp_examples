#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

void QuickSort0(vector<int> &a, int l, int r) {
	if(l >= r) return;
	int i = l, j = r;
	swap(a[i], a[(i+j)/2]);
	int x = a[i];
	while(i < j) {
		while(i < j && a[j] > x) j--;
		if(i < j) a[i++] = a[j];
		while(i < j && a[i] < x) i++;
		if(i < j) a[j--] = a[i];
	}
	a[i] = x;
	QuickSort0(a, l, i-1);
	QuickSort0(a, i+1, r);
}

void QuickSort1(vector<int> &a, int l, int r) {
	if(l >= r) return;
	int i = l, j = r;
	int x = a[(i+j)/2];
	while(i <= j) {
		while(i <= j && a[i] < x) i++;
		while(i <= j && a[j] > x) j--;
		if(i <= j) swap(a[i++], a[j--]);
	}
	QuickSort1(a, l, i-1);
	QuickSort1(a, i, r);
}

bool CheckSort(vector<int> &a, int l, int r) {
	for(int i = l+1; i < r; i++)
		if(a[i] < a[i-1]) return false;
	return true;
}

int main() {
	int n = 100;
	int test_n = 100;
	vector<int> a(n);

	for(int t = 0; t < test_n; t++) {
		for(int i = 0; i < n; i++)
			a[i] = rand() % n;
		QuickSort0(a, 0, n-1);
		assert(CheckSort(a, 0, n-1));
	}

	for(int t = 0; t < test_n; t++) {
		for(int i = 0; i < n; i++)
			a[i] = rand() % n;
		QuickSort1(a, 0, n-1);
		assert(CheckSort(a, 0, n-1));
	}

	return 0;
}
