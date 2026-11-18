#pragma once
#include <string>
#include <vector>
#include "structs.hpp"

struct Set {
    std::vector<ForwardList<std::string>> buckets;
    size_t size = 0;
    explicit Set(int n = 1009);
};

size_t hash_string(const std::string& s, int n);
bool SETADD(Set& set, const std::string& v);
bool SETDEL(Set& set, const std::string& v);
bool SET_AT(const Set& set, const std::string& v);
void print_set(const Set& set);
bool load_set_from_file(Set& set, const std::string& filename);
bool save_set_to_file(const Set& set, const std::string& filename);
void execute_query(Set& set, const std::string& query, const std::string& filename);