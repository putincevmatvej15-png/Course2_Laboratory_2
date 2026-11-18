#include "bracket_cleaner.h"
#include "structs.hpp"
#include <iostream>
using namespace std;

// Реализация
string clean_brackets(const string& input) {
    string s = input;
    vector<bool> remove(s.size(), false);
    MyStack<int> st;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '(') st.push(i);
        else if (s[i] == ')') {
            try { st.pop(); } catch (...) { remove[i] = true; }
        }
    }
    while (!st.empty()) remove[st.pop()] = true;
    string result;
    for (int i = 0; i < (int)s.size(); i++) if (!remove[i]) result += s[i];
    return result;
}

int main() {
    string s;
    cin >> s;
    cout << clean_brackets(s) << endl;
    return 0;
}