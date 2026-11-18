package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

var (
	minDiff = int(^uint(0) >> 1)
	best1   []int
	best2   []int
)

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func rec(arr []int, idx int, s1, s2 int, a, b []int) {
	if idx == len(arr) {
		d := abs(s1 - s2)
		if d < minDiff {
			minDiff = d
			best1 = append([]int(nil), a...)
			best2 = append([]int(nil), b...)
		}
		return
	}

	rec(arr, idx+1, s1+arr[idx], s2, append(a, arr[idx]), b)

	rec(arr, idx+1, s1, s2+arr[idx], a, append(b, arr[idx]))
}

func printArr(a []int) {
	fmt.Print("{")
	for i, v := range a {
		fmt.Print(v)
		if i+1 != len(a) {
			fmt.Print(", ")
		}
	}
	fmt.Print("}")
}

func sumArr(a []int) int {
	s := 0
	for _, v := range a {
		s += v
	}
	return s
}

func main() {
	var nums []int

	args := os.Args[1:]

	if len(args) > 0 && args[0] == "--" {
		args = args[1:]
	}

	if len(args) > 0 {
		for _, f := range strings.Fields(args[0]) {
			x, err := strconv.Atoi(f)
			if err == nil {
				nums = append(nums, x)
			}
		}
	} else {
		sc := bufio.NewScanner(os.Stdin)
		for sc.Scan() {
			for _, f := range strings.Fields(sc.Text()) {
				x, err := strconv.Atoi(f)
				if err == nil {
					nums = append(nums, x)
				}
			}
		}
	}

	if len(nums) == 0 {
		fmt.Fprintln(os.Stderr, "Пустой ввод")
		os.Exit(1)
	}

	rec(nums, 0, 0, 0, nil, nil)

	fmt.Print("Исходное множество: {")
	for i, v := range nums {
		fmt.Print(v)
		if i+1 != len(nums) {
			fmt.Print(", ")
		}
	}
	fmt.Println("}")

	fmt.Print("Подмножество 1: ")
	printArr(best1)
	fmt.Printf(" (сумма %d)\n", sumArr(best1))

	fmt.Print("Подмножество 2: ")
	printArr(best2)
	fmt.Printf(" (сумма %d)\n", sumArr(best2))

	fmt.Printf("Минимальная разница: %d\n", minDiff)
}
