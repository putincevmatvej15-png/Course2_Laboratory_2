package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type avlNode struct {
	key         int
	height      int
	left, right *avlNode
}

func height(n *avlNode) int {
	if n == nil {
		return 0
	}
	return n.height
}

func bf(n *avlNode) int {
	if n == nil {
		return 0
	}
	return height(n.left) - height(n.right)
}

func upd(n *avlNode) {
	l, r := height(n.left), height(n.right)
	if l > r {
		n.height = l + 1
	} else {
		n.height = r + 1
	}
}

func rotR(y *avlNode) *avlNode {
	x := y.left
	y.left = x.right
	x.right = y
	upd(y)
	upd(x)
	return x
}

func rotL(x *avlNode) *avlNode {
	y := x.right
	x.right = y.left
	y.left = x
	upd(x)
	upd(y)
	return y
}

func balance(n *avlNode) *avlNode {
	upd(n)
	b := bf(n)
	if b > 1 {
		if bf(n.left) < 0 {
			n.left = rotL(n.left)
		}
		return rotR(n)
	}
	if b < -1 {
		if bf(n.right) > 0 {
			n.right = rotR(n.right)
		}
		return rotL(n)
	}
	return n
}

func insert(n *avlNode, k int) *avlNode {
	if n == nil {
		return &avlNode{key: k, height: 1}
	}
	if k < n.key {
		n.left = insert(n.left, k)
	} else if k > n.key {
		n.right = insert(n.right, k)
	} else {
		return n
	}
	return balance(n)
}

func twoChildren(n *avlNode, out *[]int) {
	if n == nil {
		return
	}
	twoChildren(n.left, out)
	if n.left != nil && n.right != nil {
		*out = append(*out, n.key)
	}
	twoChildren(n.right, out)
}

func main() {
	var root *avlNode

	args := os.Args[1:]

	if len(args) > 0 && args[0] == "--" {
		args = args[1:]
	}

	readNumbers := func(s string) {
		for _, f := range strings.Fields(s) {
			x, err := strconv.Atoi(f)
			if err != nil || x == 0 {
				return
			}
			root = insert(root, x)
		}
	}

	if len(args) > 0 {
		readNumbers(args[0])
	} else {
		sc := bufio.NewScanner(os.Stdin)
		for sc.Scan() {
			readNumbers(sc.Text())
		}
	}

	var out []int
	twoChildren(root, &out)
	for i, v := range out {
		if i > 0 {
			fmt.Print(" ")
		}
		fmt.Print(v)
	}
	fmt.Println()
}
