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

type cuckooSlot struct {
	key   string
	value int
	used  bool
}

type CuckooHashTable struct {
	t1, t2  []cuckooSlot
	count   int
	maxKick int
}

func NewCuckooHashTable(cap int) *CuckooHashTable {
	return &CuckooHashTable{
		t1:      make([]cuckooSlot, cap),
		t2:      make([]cuckooSlot, cap),
		maxKick: 200,
	}
}

func (c *CuckooHashTable) h1(key string) int {
	var h uint64
	for i := 0; i < len(key); i++ {
		h = h*131 + uint64(key[i])
	}
	return int(h % uint64(len(c.t1)))
}

func (c *CuckooHashTable) h2(key string) int {
	var h uint64
	for i := 0; i < len(key); i++ {
		h = h*65599 + uint64(key[i])
	}
	return int(h % uint64(len(c.t2)))
}

func (c *CuckooHashTable) insertRaw(key string, value int) {
	k, v := key, value
	for kick := 0; kick < c.maxKick; kick++ {
		i1 := c.h1(k)
		if !c.t1[i1].used {
			c.t1[i1] = cuckooSlot{k, v, true}
			c.count++
			return
		}
		k, c.t1[i1].key = c.t1[i1].key, k
		v, c.t1[i1].value = c.t1[i1].value, v

		i2 := c.h2(k)
		if !c.t2[i2].used {
			c.t2[i2] = cuckooSlot{k, v, true}
			c.count++
			return
		}
		k, c.t2[i2].key = c.t2[i2].key, k
		v, c.t2[i2].value = c.t2[i2].value, v
	}
}

func (c *CuckooHashTable) rehash(newSize int) {
	old1, old2 := c.t1, c.t2
	c.t1 = make([]cuckooSlot, newSize)
	c.t2 = make([]cuckooSlot, newSize)
	c.count = 0
	for _, s := range old1 {
		if s.used {
			c.insertRaw(s.key, s.value)
		}
	}
	for _, s := range old2 {
		if s.used {
			c.insertRaw(s.key, s.value)
		}
	}
}

func (c *CuckooHashTable) insertNoRehash(key string, value int) bool {
	i := c.h1(key)
	if c.t1[i].used && c.t1[i].key == key {
		c.t1[i].value = value
		return true
	}
	j := c.h2(key)
	if c.t2[j].used && c.t2[j].key == key {
		c.t2[j].value = value
		return true
	}

	for kick := 0; kick < c.maxKick; kick++ {
		i1 := c.h1(key)
		if !c.t1[i1].used {
			c.t1[i1] = cuckooSlot{key, value, true}
			c.count++
			return true
		}
		key, c.t1[i1].key = c.t1[i1].key, key
		value, c.t1[i1].value = c.t1[i1].value, value

		i2 := c.h2(key)
		if !c.t2[i2].used {
			c.t2[i2] = cuckooSlot{key, value, true}
			c.count++
			return true
		}
		key, c.t2[i2].key = c.t2[i2].key, key
		value, c.t2[i2].value = c.t2[i2].value, value
	}
	return false
}

func (c *CuckooHashTable) Insert(key string, value int) {
	if float64(c.count)/float64(len(c.t1)+len(c.t2)) >= 0.4 {
		c.rehash(len(c.t1) * 3)
	}
	attempts := 0
	for !c.insertNoRehash(key, value) {
		c.rehash(len(c.t1) * 3)
		attempts++
		if attempts > 5 {
			fmt.Fprintf(os.Stderr, "Критическая ошибка: не удалось вставить ключ %s\n", key)
			break
		}
	}
}

func (c *CuckooHashTable) Get(key string) (int, bool) {
	i := c.h1(key)
	if c.t1[i].used && c.t1[i].key == key {
		return c.t1[i].value, true
	}
	j := c.h2(key)
	if c.t2[j].used && c.t2[j].key == key {
		return c.t2[j].value, true
	}
	return 0, false
}

func (c *CuckooHashTable) Load() float64 {
	return float64(c.count) / float64(len(c.t1)+len(c.t2))
}

type CuckooRouteTable struct {
	ht *CuckooHashTable
}

func NewCuckooRouteTable() *CuckooRouteTable {
	return &CuckooRouteTable{ht: NewCuckooHashTable(1024)}
}

func (r *CuckooRouteTable) AddRoute(from, to string, delay int) {
	r.ht.Insert(from+"->"+to, delay)
}

func (r *CuckooRouteTable) GetDelay(from, to string) (int, bool) {
	return r.ht.Get(from + "->" + to)
}

type cEdge struct {
	to, weight int
}

type cGraph struct {
	adj [][]cEdge
}

func NewCGraph(n int) *cGraph {
	return &cGraph{adj: make([][]cEdge, n)}
}

func (g *cGraph) AddEdge(u, v, w int) {
	g.adj[u] = append(g.adj[u], cEdge{v, w})
}

type cItem struct {
	dist, node int
}

type cPQ []cItem

func (p cPQ) Len() int            { return len(p) }
func (p cPQ) Less(i, j int) bool  { return p[i].dist < p[j].dist }
func (p cPQ) Swap(i, j int)       { p[i], p[j] = p[j], p[i] }
func (p *cPQ) Push(x interface{}) { *p = append(*p, x.(cItem)) }
func (p *cPQ) Pop() interface{} {
	old := *p
	n := len(old)
	it := old[n-1]
	*p = old[:n-1]
	return it
}

func cDijkstra(g *cGraph, start, end int) (int, []int) {
	n := len(g.adj)
	dist := make([]int, n)
	parent := make([]int, n)
	for i := range dist {
		dist[i] = math.MaxInt
		parent[i] = -1
	}
	dist[start] = 0

	pq := &cPQ{{0, start}}
	heap.Init(pq)

	for pq.Len() > 0 {
		it := heap.Pop(pq).(cItem)
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
				heap.Push(pq, cItem{dist[e.to], e.to})
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
		benchCuckoo()
		return
	}

	fmt.Println("=== 6.2. Кукушкино хеширование: маршруты ===")

	servers := []string{"A", "B", "C", "D", "E"}
	serverIndex := NewCuckooHashTable(1024)
	for i, s := range servers {
		serverIndex.Insert(s, i)
	}

	routes := NewCuckooRouteTable()
	g := NewCGraph(len(servers))

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

	minDelay, path := cDijkstra(g, u, v)
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

	fmt.Println("\nПроверка задержек через хеш-таблицу (кукушка):")
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

func benchCuckoo() {
	fmt.Println("Эмпирический анализ: Кукушка")
	const N = 100000
	keys := make([]string, N)
	rng := rand.New(rand.NewSource(42))
	for i := 0; i < N; i++ {
		keys[i] = "k_" + strconv.Itoa(rng.Intn(10000000))
	}

	t := NewCuckooHashTable(N * 4)

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
