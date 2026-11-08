#include "structs.hpp"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    AVLTree<int> t;

    if (argc >= 2) {
        istringstream iss(argv[1]);
        int x;
        while (iss >> x && x != 0) t.insert(x);
    } else {
        int x;
        while (cin >> x && x != 0) t.insert(x);
    }

    t.printTwoChildren();
    cout << endl;
    return 0;
}