package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func printSubarray(arr []int, start, end int) {
	fmt.Print("[")
	for i := start; i <= end; i++ {
		fmt.Print(arr[i])
		if i+1 <= end {
			fmt.Print(", ")
		}
	}
	fmt.Print("]")
}

func findSubarraysWithSum(arr []int, target int) {
	prefix := map[int][]int{}
	cur := 0
	found := false
	prefix[0] = []int{-1}

	for i, v := range arr {
		cur += v
		need := cur - target
		if starts, ok := prefix[need]; ok {
			for _, st := range starts {
				fmt.Printf("Найден подмассив с суммой %d: ", target)
				printSubarray(arr, st+1, i)
				fmt.Println()
				found = true
			}
		}
		prefix[cur] = append(prefix[cur], i)
	}

	if !found {
		fmt.Printf("Подмассивы с суммой %d не найдены.\n", target)
	}
}

func main() {
	var nums []int
	target := 0

	args := os.Args[1:]

	if len(args) > 0 && args[0] == "--" {
		args = args[1:]
	}

	switch {
	case len(args) >= 2:
		for _, f := range strings.Fields(args[0]) {
			x, err := strconv.Atoi(f)
			if err == nil {
				nums = append(nums, x)
			}
		}
		target, _ = strconv.Atoi(args[1])
	case len(args) == 1:
		target, _ = strconv.Atoi(args[0])
		nums = []int{4, -7, 1, 5, -4, 0, -3, 2, 4, 1}
	default:
		fmt.Println("Введите массив (числа через пробел, конец — Ctrl+D):")
		sc := bufio.NewScanner(os.Stdin)
		for sc.Scan() {
			for _, f := range strings.Fields(sc.Text()) {
				x, err := strconv.Atoi(f)
				if err == nil {
					nums = append(nums, x)
				}
			}
		}
		fmt.Print("Введите цель: ")
		fmt.Scan(&target)
	}

	if len(nums) == 0 {
		fmt.Fprintln(os.Stderr, "Пустой массив")
		os.Exit(1)
	}

	fmt.Print("Массив: ")
	printSubarray(nums, 0, len(nums)-1)
	fmt.Printf(", цель %d\n", target)

	findSubarraysWithSum(nums, target)
}
