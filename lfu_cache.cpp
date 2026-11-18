#include "lfu_cache.h"
#include <iostream>
using namespace std;

DoublyList<int>& LFUCache::getList(int f) {
    auto& p = freqMap[f];
    if (!p) p = make_unique<DoublyList<int>>();
    return *p;
}

void LFUCache::touch(int key) {
    auto& node = keyMap[key];
    int f = node.freq;
    getList(f).del_value(key);
    if (getList(f).empty()) {
        freqMap.erase(f);
        if (minFreq == f) ++minFreq;
    }
    getList(f + 1).push_back(key);
    node.freq = f + 1;
}

LFUCache::LFUCache(int cap) : capacity(cap), minFreq(0) {}

int LFUCache::get(int key) {
    if (!keyMap.count(key)) return -1;
    touch(key);
    return keyMap[key].value;
}

void LFUCache::set(int key, int value) {
    if (capacity <= 0) return;
    if (keyMap.count(key)) {
        keyMap[key].value = value;
        touch(key);
        return;
    }
    if ((int)keyMap.size() == capacity) {
        int victim = getList(minFreq).get(0);
        getList(minFreq).pop_front();
        if (getList(minFreq).empty()) freqMap.erase(minFreq);
        keyMap.erase(victim);
    }
    getList(1).push_back(key);
    keyMap[key] = {value, 1};
    minFreq = 1;
}

int main() {
    int cap, q;
    cout << "Введите capacity и Q: ";
    if (!(cin >> cap >> q)) return 0;

    LFUCache cache(cap);
    cout << "Введите запросы (SET x y / GET x):\n";
    for (int i = 0; i < q; ++i) {
        string cmd;
        cin >> cmd;
        if (cmd == "SET") {
            int x, y;
            cin >> x >> y;
            cache.set(x, y);
        } else if (cmd == "GET") {
            int x;
            cin >> x;
            cout << cache.get(x) << " ";
        }
    }
    cout << "\n";
    return 0;
}