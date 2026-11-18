package main

import (
	"container/heap"
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type chainNode struct {
	key   string
	value int
	next  *chainNode
}

type ChainingHashTable struct {
	buckets []*chainNode
	count   int
}

func NewChainingHashTable(cap int) *ChainingHashTable {
	return &ChainingHashTable{buckets: make([]*chainNode, cap)}
}

func (h *ChainingHashTable) hash(key string) int {
	var hv uint64
	for i := 0; i < len(key); i++ {
		hv = hv*131 + uint64(key[i])
	}
	return int(hv % uint64(len(h.buckets)))
}

func (h *ChainingHashTable) insertNoRehash(key string, value int) {
	idx := h.hash(key)
	for c := h.buckets[idx]; c != nil; c = c.next {
		if c.key == key {
			c.value = value
			return
		}
	}
	h.buckets[idx] = &chainNode{key: key, value: value, next: h.buckets[idx]}
	h.count++
}

func (h *ChainingHashTable) Insert(key string, value int) {
	if float64(h.count)/float64(len(h.buckets)) >= 0.9 {
		h.rehash(len(h.buckets) * 2)
	}
	h.insertNoRehash(key, value)
}

func (h *ChainingHashTable) rehash(newSize int) {
	old := h.buckets
	h.buckets = make([]*chainNode, newSize)
	h.count = 0
	for _, head := range old {
		for c := head; c != nil; {
			next := c.next
			h.insertNoRehash(c.key, c.value)
			c = next
		}
	}
}

func (h *ChainingHashTable) Get(key string) (int, bool) {
	idx := h.hash(key)
	for c := h.buckets[idx]; c != nil; c = c.next {
		if c.key == key {
			return c.value, true
		}
	}
	return 0, false
}

func (h *ChainingHashTable) Load() float64 {
	return float64(h.count) / float64(len(h.buckets))
}

type RouteTable struct {
	ht *ChainingHashTable
}

func NewRouteTable() *RouteTable {
	return &RouteTable{ht: NewChainingHashTable(16)}
}

func (r *RouteTable) AddRoute(from, to string, delay int) {
	r.ht.Insert(from+"->"+to, delay)
}

func (r *RouteTable) GetDelay(from, to string) (int, bool) {
	return r.ht.Get(from + "->" + to)
}

type Edge struct {
	to, weight int
}

type Graph struct {
	adj [][]Edge
}

func NewGraph(n int) *Graph {
	return &Graph{adj: make([][]Edge, n)}
}

func (g *Graph) AddEdge(u, v, w int) {
	g.adj[u] = append(g.adj[u], Edge{v, w})
}

type pqItem struct {
	dist, node int
}

type pq []pqItem

func (p pq) Len() int            { return len(p) }
func (p pq) Less(i, j int) bool  { return p[i].dist < p[j].dist }
func (p pq) Swap(i, j int)       { p[i], p[j] = p[j], p[i] }
func (p *pq) Push(x interface{}) { *p = append(*p, x.(pqItem)) }
func (p *pq) Pop() interface{} {
	old := *p
	n := len(old)
	it := old[n-1]
	*p = old[:n-1]
	return it
}

func dijkstra(g *Graph, start, end int) (int, []int) {
	n := len(g.adj)
	dist := make([]int, n)
	parent := make([]int, n)
	for i := range dist {
		dist[i] = math.MaxInt
		parent[i] = -1
	}
	dist[start] = 0

	pq := &pq{{0, start}}
	heap.Init(pq)

	for pq.Len() > 0 {
		it := heap.Pop(pq).(pqItem)
		d, u := it.dist, it.node
		if d > dist[u] {
			continue
		}
		if u == end {
			break
		}
		for _, e := range g.adj[u] {
			if dist[u]+e.weight < dist[e.to] {
				dist[e.to] = dist[u] + e.weight
				parent[e.to] = u
				heap.Push(pq, pqItem{dist[e.to], e.to})
			}
		}
	}
	if dist[end] == math.MaxInt {
		return -1, nil
	}
	var path []int
	for v := end; v != -1; v = parent[v] {
		path = append(path, v)
	}
	for i, j := 0, len(path)-1; i < j; i, j = i+1, j-1 {
		path[i], path[j] = path[j], path[i]
	}
	return dist[end], path
}

func main() {
	if len(os.Args) >= 2 && os.Args[1] == "--bench" {
		bench()
		return
	}

	fmt.Println("=== 6.2. Метод цепочек: маршруты ===")

	servers := []string{"A", "B", "C", "D", "E"}
	serverIndex := NewChainingHashTable(16)
	for i, s := range servers {
		serverIndex.Insert(s, i)
	}

	routes := NewRouteTable()
	g := NewGraph(len(servers))

	addRoute := func(a, b string, w int) {
		u, ok1 := serverIndex.Get(a)
		v, ok2 := serverIndex.Get(b)
		if !ok1 || !ok2 {
			fmt.Printf("Нет сервера %s или %s\n", a, b)
			return
		}
		g.AddEdge(u, v, w)
		routes.AddRoute(a, b, w)
		fmt.Printf("Добавлен маршрут %s -> %s (%d)\n", a, b, w)
	}

	addRoute("A", "B", 1)
	addRoute("B", "C", 2)
	addRoute("A", "C", 4)
	addRoute("C", "D", 1)
	addRoute("D", "E", 1)
	addRoute("A", "B", 5)

	start, end := "A", "E"
	u, _ := serverIndex.Get(start)
	v, _ := serverIndex.Get(end)

	minDelay, path := dijkstra(g, u, v)
	if minDelay == -1 {
		fmt.Println("Маршрут не найден.")
	} else {
		fmt.Printf("Минимальная задержка = %d (маршрут ", minDelay)
		for i, p := range path {
			fmt.Print(servers[p])
			if i+1 < len(path) {
				fmt.Print(" -> ")
			}
		}
		fmt.Println(")")
	}

	fmt.Println("\nПроверка задержек через хеш-таблицу (цепочки):")
	if d, ok := routes.GetDelay("A", "B"); ok {
		fmt.Printf("A->B = %d\n", d)
	}
	if d, ok := routes.GetDelay("B", "C"); ok {
		fmt.Printf("B->C = %d\n", d)
	}
	if d, ok := routes.GetDelay("A", "C"); ok {
		fmt.Printf("A->C = %d\n", d)
	}
	fmt.Printf("Всего маршрутов в таблице: %d\n", routes.ht.count)
}

func bench() {
	fmt.Println("Эмпирический анализ: Цепочки")
	const N = 100000
	keys := make([]string, N)
	rng := rand.New(rand.NewSource(42))
	for i := 0; i < N; i++ {
		keys[i] = "k_" + strconv.Itoa(rng.Intn(10000000))
	}

	t := NewChainingHashTable(16)

	s := time.Now()
	for _, k := range keys {
		t.Insert(k, 1)
	}
	ins := time.Since(s).Seconds() * 1000

	hits := 0
	s = time.Now()
	for _, k := range keys {
		if _, ok := t.Get(k); ok {
			hits++
		}
	}
	fnd := time.Since(s).Seconds() * 1000

	fmt.Printf("Вставка %d = %.3f мс\n", N, ins)
	fmt.Printf("Поиск   %d = %.3f мс\n", N, fnd)
	fmt.Printf("Найдено ключей: %d\n", hits)
	fmt.Printf("load = %f\n", t.Load())
}
