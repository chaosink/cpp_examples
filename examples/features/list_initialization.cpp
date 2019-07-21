#include <iostream>
using namespace std;

int main() {
	{
		int i{}; // 0
		// int i{0}; // 0
		// int i = {}; // 0
		// int i = {0}; // 0
		cout << i << endl;
	}
	cout << "----------" << endl;
	{
		int i{1}; // 1
		// int i = {1}; // 1
		cout << i << endl;
	}
	cout << "----------" << endl;
	{
		int a[2][2]; // uninitialized
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++)
				cout << a[i][j] << " ";
			cout << endl;
		}
	}
	cout << "----------" << endl;
	{
		int a[2][2]{}; // all 0
		// int a[2][2] = {}; // all 0
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++)
				cout << a[i][j] << " ";
			cout << endl;
		}
	}
	cout << "----------" << endl;
	{
		int a[2][2]{{1}}; // 1 0; 0 0
		// int a[2][2]{1}; // 1 0; 0 0; warning: suggest braces around initialization of subobject
		// int a[2][2] = {{1}}; // 1 0; 0 0
		// int a[2][2] = {1}; // 1 0; 0 0; warning: suggest braces around initialization of subobject
		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 2; j++)
				cout << a[i][j] << " ";
			cout << endl;
		}
	}
}
