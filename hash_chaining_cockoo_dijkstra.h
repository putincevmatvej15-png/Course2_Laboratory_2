#pragma once
#include <string>
#include <vector>

struct Edge {
    int to, weight;
};

struct Graph {
    int n;
    std::vector<std::vector<Edge>> adj;
    explicit Graph(int n);
    void addEdge(int u, int v, int w);
};

int dijkstra(const Graph& g, int start, int end, std::vector<int>& path);