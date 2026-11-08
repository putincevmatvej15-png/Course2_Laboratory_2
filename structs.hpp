#pragma once
#include <string>
#include <vector>
#include <deque>
#include <stdexcept>
#include <algorithm>
#include <ostream>
#include <iostream>

// 1. МАССИВ
template<class T>
class MyArray {
    std::vector<T> data;
public:
    void push_back(const T& v) { data.push_back(v); }
    void insert_at(size_t i, const T& v) {
        if (i > data.size()) throw std::out_of_range("MINSERT");
        data.insert(data.begin() + i, v);
    }
    void del_at(size_t i) {
        if (i >= data.size()) throw std::out_of_range("MDEL_AT");
        data.erase(data.begin() + i);
    }
    void del_value(const T& v) {
        for (size_t i = 0; i < data.size(); ++i)
            if (data[i] == v) { data.erase(data.begin() + i); return; }
        throw std::runtime_error("value not found");
    }
    T get(size_t i) const {
        if (i >= data.size()) throw std::out_of_range("MGET");
        return data[i];
    }
    void set(size_t i, const T& v) {
        if (i >= data.size()) throw std::out_of_range("MSET");
        data[i] = v;
    }
    size_t len() const { return data.size(); }
    void print(std::ostream& os) const {
        os << "[ ";
        for (auto& s : data) os << s << " ";
        os << "]";
    }
};

// 2. ОДНОСВЯЗНЫЙ СПИСОК 
template<class T>
class ForwardList {
    struct Node { T v; Node* next; };
    Node* head = nullptr;
    size_t sz = 0;
    Node* node_at(size_t i) const {
        Node* c = head;
        for (size_t k = 0; k < i && c; ++k) c = c->next;
        return c;
    }
public:
    ForwardList() = default;
    ForwardList(const ForwardList&) = delete;
    ForwardList& operator=(const ForwardList&) = delete;
    ForwardList(ForwardList&& o) noexcept : head(o.head), sz(o.sz) {
        o.head = nullptr; o.sz = 0;
    }
    ForwardList& operator=(ForwardList&& o) noexcept {
        if (this != &o) {
            while (head) { Node* t = head; head = head->next; delete t; }
            head = o.head; sz = o.sz;
            o.head = nullptr; o.sz = 0;
        }
        return *this;
    }
    ~ForwardList() { while (head) { Node* t = head; head = head->next; delete t; } }

    void push_front(const T& v) { head = new Node{v, head}; ++sz; }
    void push_back(const T& v) {
        Node* n = new Node{v, nullptr};
        if (!head) head = n;
        else { Node* c = head; while (c->next) c = c->next; c->next = n; }
        ++sz;
    }
    void pop_front() {
        if (!head) throw std::runtime_error("empty");
        Node* t = head; head = head->next; delete t; --sz;
    }
    void pop_back() {
        if (!head) throw std::runtime_error("empty");
        if (!head->next) { delete head; head = nullptr; --sz; return; }
        Node* c = head;
        while (c->next->next) c = c->next;
        delete c->next; c->next = nullptr; --sz;
    }
    void del_value(const T& v) {
        Node* prev = nullptr; Node* c = head;
        while (c && c->v != v) { prev = c; c = c->next; }
        if (!c) throw std::runtime_error("value not found");
        if (prev) prev->next = c->next; else head = c->next;
        delete c; --sz;
    }
    bool find(const T& v) const {
        for (Node* c = head; c; c = c->next) if (c->v == v) return true;
        return false;
    }
    template<class F>
    void forEach(F f) const {
        for (Node* c = head; c; c = c->next) f(c->v);
    }
    void print(std::ostream& os) const {
        for (Node* c = head; c; c = c->next) os << c->v << " -> ";
        os << "NULL";
    }
};

// 3. ДВУСВЯЗНЫЙ СПИСОК
template<class T>
class DoublyList {
    struct Node { T v; Node *prev, *next; };
    Node* head = nullptr;
    Node* tail = nullptr;
    size_t sz = 0;
    Node* node_at(size_t i) const {
        Node* c = head;
        for (size_t k = 0; k < i && c; ++k) c = c->next;
        return c;
    }
public:
    DoublyList() = default;
    DoublyList(const DoublyList&) = delete;
    DoublyList& operator=(const DoublyList&) = delete;
    DoublyList(DoublyList&& o) noexcept : head(o.head), tail(o.tail), sz(o.sz) {
        o.head = o.tail = nullptr; o.sz = 0;
    }
    DoublyList& operator=(DoublyList&& o) noexcept {
        if (this != &o) {
            while (head) { Node* t = head; head = head->next; delete t; }
            head = o.head; tail = o.tail; sz = o.sz;
            o.head = o.tail = nullptr; o.sz = 0;
        }
        return *this;
    }
    ~DoublyList() { while (head) { Node* t = head; head = head->next; delete t; } }

    void push_front(const T& v) {
        Node* n = new Node{v, nullptr, head};
        if (head) head->prev = n; else tail = n;
        head = n; ++sz;
    }
    void push_back(const T& v) {
        Node* n = new Node{v, tail, nullptr};
        if (tail) tail->next = n; else head = n;
        tail = n; ++sz;
    }
    void pop_front() {
        if (!head) throw std::runtime_error("empty");
        Node* t = head; head = head->next;
        if (head) head->prev = nullptr; else tail = nullptr;
        delete t; --sz;
    }
    void pop_back() {
        if (!tail) throw std::runtime_error("empty");
        Node* t = tail; tail = tail->prev;
        if (tail) tail->next = nullptr; else head = nullptr;
        delete t; --sz;
    }
    void del_value(const T& v) {
        for (Node* c = head; c; c = c->next)
            if (c->v == v) {
                if (c->prev) c->prev->next = c->next; else head = c->next;
                if (c->next) c->next->prev = c->prev; else tail = c->prev;
                delete c; --sz; return;
            }
        throw std::runtime_error("value not found");
    }
    bool find(const T& v) const {
        for (Node* c = head; c; c = c->next) if (c->v == v) return true;
        return false;
    }
    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }
    T get(size_t i) const {
        if (i >= sz) throw std::out_of_range("LGET");
        return node_at(i)->v;
    }
    void print(std::ostream& os) const {
        for (Node* c = head; c; c = c->next)
            os << c->v << (c->next ? " <-> " : " <-> NULL");
        if (!head) os << "NULL";
    }
};

//  4. СТЕК 
template<class T>
class MyStack {
    std::vector<T> s;
public:
    void push(const T& v) { s.push_back(v); }
    T pop() {
        if (s.empty()) throw std::runtime_error("empty");
        T v = s.back(); s.pop_back(); return v;
    }
    T get() const {
        if (s.empty()) throw std::runtime_error("empty");
        return s.back();
    }
    bool empty() const { return s.empty(); }
    size_t size() const { return s.size(); }
    void print(std::ostream& os) const {
        for (auto it = s.rbegin(); it != s.rend(); ++it) os << *it << " ";
    }
};

//  5. ОЧЕРЕДЬ 
template<class T>
class MyQueue {
    std::deque<T> q;
public:
    void push(const T& v) { q.push_back(v); }
    T pop() {
        if (q.empty()) throw std::runtime_error("empty");
        T v = q.front(); q.pop_front(); return v;
    }
    T get() const {
        if (q.empty()) throw std::runtime_error("empty");
        return q.front();
    }
    bool empty() const { return q.empty(); }
    size_t size() const { return q.size(); }
    void print(std::ostream& os) const {
        for (auto& v : q) os << v << " ";
    }
};

// 6. АВЛ-ДЕРЕВО 
template<class T>
class AVLTree {
    struct Node {
        T key;
        int height = 1;
        Node *left = nullptr, *right = nullptr;
        explicit Node(T k) : key(std::move(k)) {}
    };
    Node* root = nullptr;

    static int h(Node* n) { return n ? n->height : 0; }
    static int bf(Node* n) { return n ? h(n->left) - h(n->right) : 0; }
    static void upd(Node* n) { n->height = 1 + std::max(h(n->left), h(n->right)); }
    static Node* rotR(Node* y) {
        Node* x = y->left; y->left = x->right; x->right = y;
        upd(y); upd(x); return x;
    }
    static Node* rotL(Node* x) {
        Node* y = x->right; x->right = y->left; y->left = x;
        upd(x); upd(y); return y;
    }
    static Node* balance(Node* n) {
        upd(n);
        int b = bf(n);
        if (b > 1) {
            if (bf(n->left) < 0) n->left = rotL(n->left);
            return rotR(n);
        }
        if (b < -1) {
            if (bf(n->right) > 0) n->right = rotR(n->right);
            return rotL(n);
        }
        return n;
    }
    static Node* insert(Node* n, const T& k) {
        if (!n) return new Node(k);
        if (k < n->key) n->left = insert(n->left, k);
        else if (k > n->key) n->right = insert(n->right, k);
        else return n;
        return balance(n);
    }
    static Node* minNode(Node* n) { while (n->left) n = n->left; return n; }
    static Node* erase(Node* n, const T& k) {
        if (!n) return nullptr;
        if (k < n->key) n->left = erase(n->left, k);
        else if (k > n->key) n->right = erase(n->right, k);
        else {
            if (!n->left || !n->right) {
                Node* t = n->left ? n->left : n->right;
                delete n; return t;
            }
            Node* m = minNode(n->right);
            n->key = m->key;
            n->right = erase(n->right, m->key);
        }
        return balance(n);
    }
    static bool find(Node* n, const T& k) {
        while (n) {
            if (k == n->key) return true;
            n = (k < n->key) ? n->left : n->right;
        }
        return false;
    }
    static void inorder(Node* n, std::ostream& os) {
        if (!n) return;
        inorder(n->left, os);
        os << n->key << "(" << n->height << ":" << bf(n) << ") ";
        inorder(n->right, os);
    }
    static void twoChildren(Node* n, std::ostream& os) {
        if (!n) return;
        twoChildren(n->left, os);
        if (n->left && n->right) os << n->key << " ";
        twoChildren(n->right, os);
    }
    static void freeTree(Node* n) {
        if (!n) return; freeTree(n->left); freeTree(n->right); delete n;
    }
public:
    AVLTree() = default;
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;
    ~AVLTree() { freeTree(root); }

    void insert(const T& k) { root = insert(root, k); }
    void erase(const T& k) {
        if (!find(root, k)) throw std::runtime_error("key not found");
        root = erase(root, k);
    }
    bool find(const T& k) const { return find(root, k); }
    void print(std::ostream& os) const { inorder(root, os); }
    void printTwoChildren(std::ostream& os = std::cout) const { twoChildren(root, os); }
};