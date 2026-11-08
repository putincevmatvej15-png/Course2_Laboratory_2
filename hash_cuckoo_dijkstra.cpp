#include "structs.hpp"
#include <iostream>
#include <utility> 
#include <vector>
#include <string>
#include <queue>
#include <climits>
#include <chrono>
#include <random>
using namespace std;

// Кукушкино хеширование
class CuckooHashTable {
    struct Slot { string key; int value; bool used = false; };
    vector<Slot> t1, t2;
    size_t count = 0;
    static const int MAX_KICKS = 500;

    size_t h1(const string& key) const {
        unsigned long long h = 0;
        for (char c : key) h = h * 131 + (unsigned char)c;
        return (size_t)(h % t1.size());
    }
    size_t h2(const string& key) const {
        unsigned long long h = 0;
        for (char c : key) h = h * 65599 + (unsigned char)c;
        return (size_t)(h % t2.size());
    }

    void rehash(size_t newSize) {
        vector<Slot> old1 = std::move(t1), old2 = std::move(t2);
        t1.assign(newSize, Slot{});
        t2.assign(newSize, Slot{});
        count = 0;
        for (auto& s : old1) if (s.used) insertNoRehash(s.key, s.value);
        for (auto& s : old2) if (s.used) insertNoRehash(s.key, s.value);
    }

    void insertNoRehash(string key, int value) {
        for (int kick = 0; kick < MAX_KICKS; ++kick) {
            size_t i = h1(key);
            if (!t1[i].used) { t1[i] = {key, value, true}; ++count; return; }
            std::swap(key, t1[i].key);
            std::swap(value, t1[i].value);

            size_t j = h2(key);
            if (!t2[j].used) { t2[j] = {key, value, true}; ++count; return; }
            std::swap(key, t2[j].key);
            std::swap(value, t2[j].value);
        }
        rehash(t1.size() * 2);
        insertNoRehash(key, value);
    }

public:
    explicit CuckooHashTable(size_t cap = 16) : t1(cap), t2(cap) {}

    void insert(const string& key, int value) {
        if ((double)count / (t1.size() + t2.size()) >= 0.9) rehash(t1.size() * 2);
        insertNoRehash(key, value);
    }

    int get(const string& key, int def = -1) const {
        size_t i = h1(key);
        if (t1[i].used && t1[i].key == key) return t1[i].value;
        size_t j = h2(key);
        if (t2[j].used && t2[j].key == key) return t2[j].value;
        return def;
    }

    bool contains(const string& key) const { return get(key, -1) != -1; }
    size_t size() const { return count; }
    size_t capacity() const { return t1.size() + t2.size(); }
    double load() const { return (double)count / (t1.size() + t2.size()); }
};

// Граф
struct Edge { int to, weight; };
struct Graph {
    int n;
    vector<vector<Edge>> adj;
    explicit Graph(int n) : n(n), adj(n) {}
    void addEdge(int u, int v, int w) { adj[u].push_back({v, w}); }
};

// Маршруты в Хеш-таблице
class RouteTable {
    CuckooHashTable ht;
public:
    void addRoute(const string& from, const string& to, int delay) {
        ht.insert(from + "->" + to, delay);
    }
    int getDelay(const string& from, const string& to) const {
        return ht.get(from + "->" + to, -1);
    }
    bool hasRoute(const string& from, const string& to) const {
        return ht.contains(from + "->" + to);
    }
    size_t size() const { return ht.size(); }
    double load() const { return ht.load(); }
};

// Дейкстра
int dijkstra(const Graph& g, int start, int end, vector<int>& path) {
    vector<int> dist(g.n, INT_MAX), parent(g.n, -1);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == end) break;
        for (auto& e : g.adj[u]) {
            if (dist[u] + e.weight < dist[e.to]) {
                dist[e.to] = dist[u] + e.weight;
                parent[e.to] = u;
                pq.push({dist[e.to], e.to});
            }
        }
    }
    if (dist[end] == INT_MAX) return -1;
    path.clear();
    for (int v = end; v != -1; v = parent[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    return dist[end];
}

int main(int argc, char* argv[]) {
    if (argc >= 2 && string(argv[1]) == "--bench") {
        cout << "=== Эмпирический анализ: КУКУШКА ===\n";
        const int N = 100000;
        vector<string> keys(N);
        mt19937_64 rng(42);
        for (int i = 0; i < N; ++i) keys[i] = "k_" + to_string(rng() % 10000000);

        CuckooHashTable t;
        auto s = chrono::high_resolution_clock::now();
        for (auto& k : keys) t.insert(k, 1);
        auto e = chrono::high_resolution_clock::now();
        double ins = chrono::duration<double, milli>(e - s).count();

        int hits = 0;
        s = chrono::high_resolution_clock::now();
        for (auto& k : keys) if (t.get(k) != -1) ++hits;
        e = chrono::high_resolution_clock::now();
        double fnd = chrono::duration<double, milli>(e - s).count();

        cout << "Вставка " << N << " = " << ins << " мс\n";
        cout << "Поиск   " << N << " = " << fnd << " мс\n";
        cout << "load = " << t.load() << "\n";
        return 0;
    }

    cout << "=== 6.2. Кукушкино хеширование: маршруты ===\n";

    vector<string> servers = {"A", "B", "C", "D", "E"};
    CuckooHashTable serverIndex;
    for (int i = 0; i < (int)servers.size(); ++i) serverIndex.insert(servers[i], i);

    RouteTable routes;
    Graph g(servers.size());

    auto addRoute = [&](const string& a, const string& b, int w) {
        int u = serverIndex.get(a);
        int v = serverIndex.get(b);
        if (u == -1 || v == -1) { cerr << "Нет сервера " << a << " или " << b << "\n"; return; }
        g.addEdge(u, v, w);
        routes.addRoute(a, b, w);
        cout << "Добавлен маршрут " << a << " -> " << b << " (" << w << ")\n";
    };

    addRoute("A", "B", 1);
    addRoute("B", "C", 2);
    addRoute("A", "C", 4);
    addRoute("C", "D", 1);
    addRoute("D", "E", 1);
    addRoute("A", "B", 5);   

    string start = "A", end = "E";
    int u = serverIndex.get(start), v = serverIndex.get(end);
    if (u == -1 || v == -1) { cout << "Ошибка: сервер не найден\n"; return 0; }

    vector<int> path;
    int minDelay = dijkstra(g, u, v, path);

    if (minDelay == -1) {
        cout << "Маршрут не найден.\n";
    } else {
        cout << "Минимальная задержка = " << minDelay << " (маршрут ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << servers[path[i]];
            if (i + 1 < path.size()) cout << " -> ";
        }
        cout << ")\n";
    }

    cout << "\nПроверка задержек через хеш-таблицу (кукушка):\n";
    cout << "A->B = " << routes.getDelay("A", "B") << "\n";
    cout << "B->C = " << routes.getDelay("B", "C") << "\n";
    cout << "A->C = " << routes.getDelay("A", "C") << "\n";
    cout << "Всего маршрутов в таблице: " << routes.size() << "\n";
    return 0;
}