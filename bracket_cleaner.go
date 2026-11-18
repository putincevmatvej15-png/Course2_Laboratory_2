package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func main() {
	scanner := bufio.NewScanner(os.Stdin)
	if scanner.Scan() {
		input := scanner.Text()
		input = strings.TrimSpace(input)

		input = strings.Trim(input, `"`)
		input = strings.Trim(input, `'`)

		s := []rune(input)
		remove := make([]bool, len(s))
		var stack []int

		for i, ch := range s {
			if ch == '(' {
				stack = append(stack, i)
			} else if ch == ')' {
				if len(stack) > 0 {
					stack = stack[:len(stack)-1]
				} else {
					remove[i] = true
				}
			}
		}

		for _, idx := range stack {
			remove[idx] = true
		}

		var result strings.Builder
		for i, ch := range s {
			if !remove[i] {
				result.WriteRune(ch)
			}
		}

		fmt.Printf("\"%s\"\n", result.String())
	}
}
