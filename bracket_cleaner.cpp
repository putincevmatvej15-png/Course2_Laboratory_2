#include "structs.hpp"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
    string s;
    cin >> s;
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
    cout << result << endl;
}