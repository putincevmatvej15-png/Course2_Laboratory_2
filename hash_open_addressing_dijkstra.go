package main

import (
	"container/heap"
	"fmt"
	"math"
)

type HashEntry struct {
	key        string
	value      int
	isOccupied bool
}

type HashTable struct {
	buckets  []HashEntry
	size     int
	capacity int
}

func NewHashTable(capacity int) *HashTable {
	return &HashTable{
		buckets:  make([]HashEntry, capacity),
		capacity: capacity,
	}
}

func hashFunc(key string, capacity int) int {
	var h uint32
	for i := 0; i < len(key); i++ {
		h = h*31 + uint32(key[i])
	}
	return int(h % uint32(capacity))
}

func (h *HashTable) resize() {
	oldCap := h.capacity
	oldBuckets := h.buckets

	h.capacity *= 2
	h.buckets = make([]HashEntry, h.capacity)
	h.size = 0

	for i := 0; i < oldCap; i++ {
		if oldBuckets[i].isOccupied {
			h.Insert(oldBuckets[i].key, oldBuckets[i].value)
		}
	}
}

func (h *HashTable) Insert(key string, value int) {
	if h.size >= int(float64(h.capacity)*0.7) {
		h.resize()
	}
	idx := hashFunc(key, h.capacity)
	for h.buckets[idx].isOccupied {
		if h.buckets[idx].key == key {
			h.buckets[idx].value = value
			return
		}
		idx = (idx + 1) % h.capacity
	}
	h.buckets[idx] = HashEntry{key, value, true}
	h.size++
}

func (h *HashTable) Get(key string) (int, bool) {
	idx := hashFunc(key, h.capacity)
	start := idx
	for h.buckets[idx].isOccupied {
		if h.buckets[idx].key == key {
			return h.buckets[idx].value, true
		}
		idx = (idx + 1) % h.capacity
		if idx == start {
			break
		}
	}
	return 0, false
}

type oaEdge struct {
	to, weight int
}

type oaGraph struct {
	adj [][]oaEdge
}

func NewOAGraph(n int) *oaGraph {
	return &oaGraph{adj: make([][]oaEdge, n)}
}

func (g *oaGraph) AddEdge(u, v, w int) {
	g.adj[u] = append(g.adj[u], oaEdge{v, w})
}

type oaItem struct {
	dist, node int
}

type oaPQ []oaItem

func (p oaPQ) Len() int            { return len(p) }
func (p oaPQ) Less(i, j int) bool  { return p[i].dist < p[j].dist }
func (p oaPQ) Swap(i, j int)       { p[i], p[j] = p[j], p[i] }
func (p *oaPQ) Push(x interface{}) { *p = append(*p, x.(oaItem)) }
func (p *oaPQ) Pop() interface{} {
	old := *p
	n := len(old)
	it := old[n-1]
	*p = old[:n-1]
	return it
}

func oaDijkstra(g *oaGraph, start, end int) (int, bool) {
	n := len(g.adj)
	dist := make([]int, n)
	for i := range dist {
		dist[i] = math.MaxInt
	}
	dist[start] = 0
	pq := &oaPQ{{0, start}}
	heap.Init(pq)

	for pq.Len() > 0 {
		it := heap.Pop(pq).(oaItem)
		d, u := it.dist, it.node
		if d > dist[u] {
			continue
		}
		if u == end {
			return dist[end], true
		}
		for _, e := range g.adj[u] {
			if dist[u]+e.weight < dist[e.to] {
				dist[e.to] = dist[u] + e.weight
				heap.Push(pq, oaItem{dist[e.to], e.to})
			}
		}
	}
	return -1, false
}

func reachable(g *oaGraph, start, end int) bool {
	visited := make([]bool, len(g.adj))
	queue := []int{start}
	visited[start] = true
	for len(queue) > 0 {
		u := queue[0]
		queue = queue[1:]
		if u == end {
			return true
		}
		for _, e := range g.adj[u] {
			if !visited[e.to] {
				visited[e.to] = true
				queue = append(queue, e.to)
			}
		}
	}
	return false
}

func main() {
	servers := []string{"A", "B", "C"}
	serverMap := NewHashTable(20)
	for i, s := range servers {
		serverMap.Insert(s, i)
	}

	g := NewOAGraph(len(servers))
	a, _ := serverMap.Get("A")
	b, _ := serverMap.Get("B")
	c, _ := serverMap.Get("C")

	g.AddEdge(a, b, 1)
	g.AddEdge(b, c, 2)
	g.AddEdge(a, c, 4)

	startServer, endServer := "A", "C"
	fmt.Println("--- Открытая адресация ---")
	u, ok1 := serverMap.Get(startServer)
	v, ok2 := serverMap.Get(endServer)

	if ok1 && ok2 {
		if !reachable(g, u, v) {
			fmt.Println("Маршрут недостижим.")
		} else {
			delay, found := oaDijkstra(g, u, v)
			if found {
				fmt.Printf("Минимальная задержка: %d\n", delay)
			} else {
				fmt.Println("Маршрут не найден.")
			}
		}
	} else {
		fmt.Println("Ошибка.")
	}
}
