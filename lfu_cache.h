#pragma once
#include <unordered_map>
#include <memory>
#include "structs.hpp"

class LFUCache {
    int capacity, minFreq;
    struct Node { int value; int freq; };
    std::unordered_map<int, Node> keyMap;
    std::unordered_map<int, std::unique_ptr<DoublyList<int>>> freqMap;

    DoublyList<int>& getList(int f);
    void touch(int key);
public:
    LFUCache(int cap);
    int get(int key);
    void set(int key, int value);
};