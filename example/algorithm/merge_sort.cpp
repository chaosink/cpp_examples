#include <cassert>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <vector>
using namespace std;

void MergeSort(vector<int> &a, int l, int r) {
    if(l + 1 >= r)
        return;
    int m = l + (r - l) / 2;
    MergeSort(a, l, m);
    MergeSort(a, m, r);
    vector<int> temp(r - l);
    int i = l, j = m, k = 0;
    while(i != m || j != r) {
        while(i != m && (j == r || a[i] < a[j]))
            temp[k++] = a[i++];
        while(j != r && (i == m || a[i] >= a[j]))
            temp[k++] = a[j++];
    }
    copy(temp.begin(), temp.end(), a.begin() + l);
}

bool CheckSort(vector<int> &a, int l, int r) {
    for(int i = l + 1; i < r; i++)
        if(a[i] < a[i - 1])
            return false;
    return true;
}

int main() {
    int n = 100;
    int test_n = 100;
    vector<int> a(n);

    srand(time(nullptr));

    for(int t = 0; t < test_n; t++) {
        for(int i = 0; i < n; i++)
            a[i] = rand() % n;
        vector<int> b = a;
        MergeSort(a, 0, n);
        sort(b.begin(), b.end());
        assert(a == b);
    }

    return 0;
}
