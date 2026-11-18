#include "subarray_sum_finder.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
using namespace std;

void print_subarray(const MyArray<int>& arr, int start, int end) {
    cout << "[";
    for (int i = start; i <= end; ++i) {
        cout << arr.get(i);
        if (i + 1 <= end) cout << ", ";
    }
    cout << "]";
}

void find_subarrays_with_sum(const MyArray<int>& arr, int target) {
    unordered_map<int, vector<int>> prefix;
    int cur = 0;
    bool found = false;
    prefix[0].push_back(-1);

    for (size_t i = 0; i < arr.len(); ++i) {
        cur += arr.get(i);
        int need = cur - target;
        if (prefix.count(need)) {
            for (int st : prefix[need]) {
                cout << "Найден подмассив с суммой " << target << ": ";
                print_subarray(arr, st + 1, (int)i);
                cout << "\n";
                found = true;
            }
        }
        prefix[cur].push_back((int)i);
    }

    if (!found) {
        cout << "Подмассивы с суммой " << target << " не найдены.\n";
    }
}

int main(int argc, char* argv[]) {
    vector<int> nums;
    int target = 0;

    if (argc >= 3) {
        istringstream iss(argv[1]);
        int x;
        while (iss >> x) nums.push_back(x);
        target = stoi(argv[2]);
    } else if (argc == 2) {
        target = stoi(argv[1]);
        nums = {4, -7, 1, 5, -4, 0, -3, 2, 4, 1};
    } else {
        cout << "Введите массив (числа через пробел, конец — Ctrl+D):\n";
        int x;
        while (cin >> x) nums.push_back(x);
        cout << "Введите цель: ";
        cin >> target;
    }

    if (nums.empty()) {
        cerr << "Пустой массив\n";
        return 1;
    }

    MyArray<int> arr;
    for (int x : nums) arr.push_back(x);

    cout << "Массив: ";
    print_subarray(arr, 0, (int)arr.len() - 1);
    cout << ", цель " << target << "\n";

    find_subarrays_with_sum(arr, target);
    return 0;
}