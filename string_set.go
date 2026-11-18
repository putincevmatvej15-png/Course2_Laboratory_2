package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type Set struct {
	buckets [][]string
	size    int
}

func NewSet(n int) *Set {
	return &Set{buckets: make([][]string, n)}
}

func hashString(s string, n int) uint64 {
	var h uint64
	for i := 0; i < len(s); i++ {
		h = h*131 + uint64(s[i])
	}
	return h % uint64(n)
}

func (s *Set) Add(v string) bool {
	idx := hashString(v, len(s.buckets))
	for _, x := range s.buckets[idx] {
		if x == v {
			return false
		}
	}
	s.buckets[idx] = append(s.buckets[idx], v)
	s.size++
	return true
}

func (s *Set) Del(v string) bool {
	idx := hashString(v, len(s.buckets))
	for i, x := range s.buckets[idx] {
		if x == v {
			s.buckets[idx] = append(s.buckets[idx][:i], s.buckets[idx][i+1:]...)
			s.size--
			return true
		}
	}
	return false
}

func (s *Set) At(v string) bool {
	idx := hashString(v, len(s.buckets))
	for _, x := range s.buckets[idx] {
		if x == v {
			return true
		}
	}
	return false
}

func (s *Set) Print() {
	if s.size == 0 {
		fmt.Println("Множество пусто")
		return
	}
	fmt.Printf("Множество [%d]: {", s.size)
	first := true
	for _, b := range s.buckets {
		for _, v := range b {
			if !first {
				fmt.Print(", ")
			}
			fmt.Print(v)
			first = false
		}
	}
	fmt.Println("}")
}

func (s *Set) LoadFromFile(filename string) bool {
	f, err := os.Open(filename)
	if err != nil {
		fmt.Printf("Ошибка: Не удалось открыть файл %s\n", filename)
		fmt.Println("Файл не найден — будет создан новый при сохранении")
		return false
	}
	defer f.Close()
	sc := bufio.NewScanner(f)
	loaded := 0
	for sc.Scan() {
		line := sc.Text()
		if line != "" && s.Add(line) {
			loaded++
		}
	}
	fmt.Printf("Загружено %d элементов из файла %s\n", loaded, filename)
	return true
}

func (s *Set) SaveToFile(filename string) bool {
	f, err := os.Create(filename)
	if err != nil {
		fmt.Printf("Ошибка: Не удалось создать файл %s\n", filename)
		return false
	}
	defer f.Close()
	w := bufio.NewWriter(f)
	saved := 0
	for _, b := range s.buckets {
		for _, v := range b {
			fmt.Fprintln(w, v)
			saved++
		}
	}
	w.Flush()
	fmt.Printf("Сохранено %d элементов в файл %s\n", saved, filename)
	return true
}

func executeQuery(set *Set, query, filename string) {
	parts := strings.SplitN(query, " ", 2)
	command := parts[0]
	value := ""
	if len(parts) > 1 {
		value = parts[1]
	}

	switch command {
	case "SETADD":
		if value == "" {
			fmt.Println("Ошибка: SETADD требует значение")
			return
		}
		if set.Add(value) {
			fmt.Printf("Элемент '%s' добавлен\n", value)
			set.SaveToFile(filename)
		} else {
			fmt.Printf("Элемент '%s' уже существует\n", value)
		}
	case "SETDEL":
		if value == "" {
			fmt.Println("Ошибка: SETDEL требует значение")
			return
		}
		if set.Del(value) {
			fmt.Printf("Элемент '%s' удалён\n", value)
			set.SaveToFile(filename)
		} else {
			fmt.Printf("Элемент '%s' не найден\n", value)
		}
	case "SET_AT":
		if value == "" {
			fmt.Println("Ошибка: SET_AT требует значение")
			return
		}
		if set.At(value) {
			fmt.Printf("Элемент '%s' присутствует\n", value)
		} else {
			fmt.Printf("Элемент '%s' отсутствует\n", value)
		}
	case "PRINT":
		set.Print()
	case "SAVE":
		set.SaveToFile(filename)
	default:
		fmt.Printf("Ошибка: неизвестная команда '%s'\n", command)
		fmt.Println("Доступно: SETADD, SETDEL, SET_AT, PRINT, SAVE")
	}
}

func main() {
	args := os.Args[1:]
	filename, query := "", ""
	for i := 0; i < len(args); i++ {
		switch args[i] {
		case "--file":
			if i+1 < len(args) {
				i++
				filename = args[i]
			}
		case "--query":
			if i+1 < len(args) {
				i++
				query = args[i]
			}
		}
	}

	if filename == "" || query == "" {
		fmt.Println("Ошибка: нужны --file и --query")
		return
	}

	mySet := NewSet(1009)
	mySet.LoadFromFile(filename)
	executeQuery(mySet, query, filename)
}
