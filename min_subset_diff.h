#pragma once
#include <vector>
#include "structs.hpp"

extern int min_diff;
extern MyArray<int> best1;
extern MyArray<int> best2;

void rec(int arr[], int idx, int n, int s1, int s2, MyArray<int>& a, MyArray<int>& b);
void print_arr(const MyArray<int>& a);
int sum_arr(const MyArray<int>& a);