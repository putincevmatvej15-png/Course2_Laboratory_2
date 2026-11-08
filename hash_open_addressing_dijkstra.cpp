#include "structs.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <climits>
using namespace std;

struct HashEntry {
    string key;
    int value;
    bool isOccupied;
};

struct HashTable {
    HashEntry* buckets;
    int size;
    int capacity;
};

unsigned int hash_func(const string& key, int capacity) {
    unsigned int hash = 0;
    for (char c : key) hash = (hash * 31) + c;
    return hash % capacity;
}

HashTable* create_hashtable(int capacity = 20) {
    HashTable* ht = new HashTable;
    ht->capacity = capacity;
    ht->size = 0;
    ht->buckets = new HashEntry[capacity];
    for (int i = 0; i < capacity; i++) ht->buckets[i].isOccupied = false;
    return ht;
}

void resize_hashtable(HashTable* ht) {
    int oldCapacity = ht->capacity;
    HashEntry* oldBuckets = ht->buckets;

    ht->capacity *= 2;
    ht->buckets = new HashEntry[ht->capacity];
    ht->size = 0;
    for (int i = 0; i < ht->capacity; i++) ht->buckets[i].isOccupied = false;

    for (int i = 0; i < oldCapacity; i++) {
        if (oldBuckets[i].isOccupied) {
            string key = oldBuckets[i].key;
            int val = oldBuckets[i].value;
            unsigned int idx = hash_func(key, ht->capacity);
            while (ht->buckets[idx].isOccupied) idx = (idx + 1) % ht->capacity;
            ht->buckets[idx].key = key;
            ht->buckets[idx].value = val;
            ht->buckets[idx].isOccupied = true;
            ht->size++;
        }
    }
    delete[] oldBuckets;
}

void ht_insert(HashTable* ht, const string& key, int value) {
    if (ht->size >= ht->capacity * 0.7) resize_hashtable(ht);
    unsigned int index = hash_func(key, ht->capacity);
    while (ht->buckets[index].isOccupied) {
        if (ht->buckets[index].key == key) { ht->buckets[index].value = value; return; }
        index = (index + 1) % ht->capacity;
    }
    ht->buckets[index].key = key;
    ht->buckets[index].value = value;
    ht->buckets[index].isOccupied = true;
    ht->size++;
}

int ht_get(HashTable* ht, const string& key) {
    unsigned int index = hash_func(key, ht->capacity);
    unsigned int start_index = index;
    while (ht->buckets[index].isOccupied) {
        if (ht->buckets[index].key == key) return ht->buckets[index].value;
        index = (index + 1) % ht->capacity;
        if (index == start_index) break;
    }
    return -1;
}

void destroy_hashtable(HashTable* ht) {
    delete[] ht->buckets;
    delete ht;
}

using Edge = pair<int, int>;
using PQ_Pair = pair<int, int>;

struct WeightedGraph {
    int num_vertices;
    vector<Edge>* adj;
};

void init_w_graph(WeightedGraph* g, int num_vertices) {
    g->num_vertices = num_vertices;
    g->adj = new vector<Edge>[num_vertices];
}

void add_w_edge(WeightedGraph* g, int u, int v, int weight) {
    g->adj[u].push_back({v, weight});
}

void destroy_w_graph(WeightedGraph* g) {
    delete[] g->adj;
}

int dijkstra(WeightedGraph* g, int start_idx, int end_idx) {
    MyArray<int> dist;
    for (int i = 0; i < g->num_vertices; ++i) dist.push_back(INT_MAX);
    priority_queue<PQ_Pair, vector<PQ_Pair>, greater<PQ_Pair>> pq;

    dist.set(start_idx, 0);
    pq.push({0, start_idx});

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist.get(u)) continue;
        if (u == end_idx) return dist.get(end_idx);

        for (const auto& edge : g->adj[u]) {
            int v = edge.first;
            int weight = edge.second;
            if (dist.get(u) + weight < dist.get(v)) {
                dist.set(v, dist.get(u) + weight);
                pq.push({dist.get(v), v});
            }
        }
    }
    return -1;
}

bool reachable(WeightedGraph* g, int start, int end) {
    vector<bool> visited(g->num_vertices, false);
    MyQueue<int> q;
    q.push(start); visited[start] = true;
    while (!q.empty()) {
        int u = q.pop();
        if (u == end) return true;
        for (auto& e : g->adj[u]) {
            int v = e.first;
            if (!visited[v]) { visited[v] = true; q.push(v); }
        }
    }
    return false;
}

int main() {
    vector<string> servers = {"A", "B", "C"};
    HashTable* server_map = create_hashtable(20);

    int idx_counter = 0;
    for (const auto& s : servers) ht_insert(server_map, s, idx_counter++);

    WeightedGraph g;
    init_w_graph(&g, servers.size());

    add_w_edge(&g, ht_get(server_map, "A"), ht_get(server_map, "B"), 1);
    add_w_edge(&g, ht_get(server_map, "B"), ht_get(server_map, "C"), 2);
    add_w_edge(&g, ht_get(server_map, "A"), ht_get(server_map, "C"), 4);

    string start_server = "A", end_server = "C";
    cout << "--- Открытая адресация ---" << endl;
    int u = ht_get(server_map, start_server);
    int v = ht_get(server_map, end_server);

    if (u != -1 && v != -1) {
        if (!reachable(&g, u, v)) {
            cout << "Маршрут недостижим.\n";
        } else {
            int min_delay = dijkstra(&g, u, v);
            if (min_delay != -1) cout << "Минимальная задержка: " << min_delay << endl;
            else                 cout << "Маршрут не найден." << endl;
        }
    } else {
        cout << "Ошибка." << endl;
    }

    destroy_w_graph(&g);
    destroy_hashtable(server_map);
    return 0;
}