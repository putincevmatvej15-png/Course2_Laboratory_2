#include "min_subset_diff.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <climits>
using namespace std;

int min_diff = INT_MAX;
MyArray<int> best1, best2;

void rec(int arr[], int idx, int n, int s1, int s2, MyArray<int>& a, MyArray<int>& b) {
    if (idx == n) {
        int d = abs(s1 - s2);
        if (d < min_diff) {
            min_diff = d;
            best1 = MyArray<int>();
            best2 = MyArray<int>();
            for (size_t i = 0; i < a.len(); ++i) best1.push_back(a.get(i));
            for (size_t i = 0; i < b.len(); ++i) best2.push_back(b.get(i));
        }
        return;
    }
    a.push_back(arr[idx]);
    rec(arr, idx + 1, n, s1 + arr[idx], s2, a, b);
    a.del_at(a.len() - 1);

    b.push_back(arr[idx]);
    rec(arr, idx + 1, n, s1, s2 + arr[idx], a, b);
    b.del_at(b.len() - 1);
}

void print_arr(const MyArray<int>& a) {
    cout << "{";
    for (size_t i = 0; i < a.len(); ++i) {
        cout << a.get(i);
        if (i + 1 != a.len()) cout << ", ";
    }
    cout << "}";
}

int sum_arr(const MyArray<int>& a) {
    int s = 0;
    for (size_t i = 0; i < a.len(); ++i) s += a.get(i);
    return s;
}

int main(int argc, char* argv[]) {
    vector<int> nums;
    if (argc >= 2) {
        istringstream iss(argv[1]);
        int x;
        while (iss >> x) nums.push_back(x);
    } else {
        int x;
        while (cin >> x) nums.push_back(x);
    }
    if (nums.empty()) { cerr << "Пустой ввод\n"; return 1; }

    int* arr = new int[nums.size()];
    for (size_t i = 0; i < nums.size(); ++i) arr[i] = nums[i];

    MyArray<int> a, b;
    rec(arr, 0, (int)nums.size(), 0, 0, a, b);

    cout << "Исходное множество: {";
    for (size_t i = 0; i < nums.size(); ++i) {
        cout << nums[i];
        if (i + 1 != nums.size()) cout << ", ";
    }
    cout << "}\n";
    cout << "Подмножество 1: "; print_arr(best1);
    cout << " (сумма " << sum_arr(best1) << ")\n";
    cout << "Подмножество 2: "; print_arr(best2);
    cout << " (сумма " << sum_arr(best2) << ")\n";
    cout << "Минимальная разница: " << min_diff << "\n";
    delete[] arr;
    return 0;
}