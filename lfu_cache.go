package main

import (
	"container/list"
	"fmt"
	"os"
)

type lfuNode struct {
	value int
	freq  int
}

type LFUCache struct {
	capacity int
	minFreq  int
	keyMap   map[int]*lfuNode
	freqMap  map[int]*list.List
}

func NewLFUCache(cap int) *LFUCache {
	return &LFUCache{
		capacity: cap,
		keyMap:   make(map[int]*lfuNode),
		freqMap:  make(map[int]*list.List),
	}
}

func (c *LFUCache) getList(f int) *list.List {
	l, ok := c.freqMap[f]
	if !ok {
		l = list.New()
		c.freqMap[f] = l
	}
	return l
}

func (c *LFUCache) touch(key int) {
	node := c.keyMap[key]
	f := node.freq

	l := c.getList(f)
	for e := l.Front(); e != nil; e = e.Next() {
		if e.Value.(int) == key {
			l.Remove(e)
			break
		}
	}
	if l.Len() == 0 {
		delete(c.freqMap, f)
		if c.minFreq == f {
			c.minFreq++
		}
	}

	c.getList(f + 1).PushBack(key)
	node.freq = f + 1
}

func (c *LFUCache) Get(key int) int {
	node, ok := c.keyMap[key]
	if !ok {
		return -1
	}
	c.touch(key)
	return node.value
}

func (c *LFUCache) Set(key, value int) {
	if c.capacity <= 0 {
		return
	}

	if node, ok := c.keyMap[key]; ok {
		node.value = value
		c.touch(key)
		return
	}

	if len(c.keyMap) == c.capacity {
		l := c.getList(c.minFreq)
		front := l.Front()
		if front == nil {
			return
		}
		victim := front.Value.(int)
		l.Remove(front)
		if l.Len() == 0 {
			delete(c.freqMap, c.minFreq)
		}
		delete(c.keyMap, victim)
	}

	c.getList(1).PushBack(key)
	c.keyMap[key] = &lfuNode{value: value, freq: 1}
	c.minFreq = 1
}

func main() {
	originalStdout := os.Stdout
	devNull, _ := os.Open(os.DevNull)
	os.Stdout = devNull

	var cap, q int
	fmt.Print("Введите capacity и Q: ")
	fmt.Scan(&cap, &q)

	cache := NewLFUCache(cap)
	fmt.Println("Введите запросы (SET x y / GET x):")

	var out []string
	for i := 0; i < q; i++ {
		var cmd string
		fmt.Scan(&cmd)
		if cmd == "SET" {
			var x, y int
			fmt.Scan(&x, &y)
			cache.Set(x, y)
		} else if cmd == "GET" {
			var x int
			fmt.Scan(&x)
			val := cache.Get(x)
			out = append(out, fmt.Sprintf("%d", val))
		}
	}

	os.Stdout = originalStdout

	for i, v := range out {
		if i > 0 {
			fmt.Print(" ")
		}
		fmt.Print(v)
	}
	fmt.Println()
}
