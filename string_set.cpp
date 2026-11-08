#include "structs.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

struct Set {
    vector<ForwardList<string>> buckets;
    size_t size = 0;
    explicit Set(int n = 1009) : buckets(n) {}
};

size_t hash_string(const string& s, int n) {
    unsigned long long h = 0;
    for (char c : s) h = h * 131 + (unsigned char)c;
    return h % n;
}

bool SETADD(Set& set, const string& v) {
    auto& b = set.buckets[hash_string(v, (int)set.buckets.size())];
    if (b.find(v)) return false;
    b.push_front(v);
    ++set.size;
    return true;
}

bool SETDEL(Set& set, const string& v) {
    auto& b = set.buckets[hash_string(v, (int)set.buckets.size())];
    try { b.del_value(v); --set.size; return true; }
    catch (...) { return false; }
}

bool SET_AT(const Set& set, const string& v) {
    return set.buckets[hash_string(v, (int)set.buckets.size())].find(v);
}

void print_set(const Set& set) {
    if (set.size == 0) { cout << "Множество пусто\n"; return; }
    cout << "Множество [" << set.size << "]: {";
    bool first = true;
    for (auto& b : set.buckets) {
        b.forEach([&](const string& v) {
            if (!first) cout << ", ";
            cout << v;
            first = false;
        });
    }
    cout << "}\n";
}

bool load_set_from_file(Set& set, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось открыть файл " << filename << "\n";
        return false;
    }
    string line;
    int loaded = 0;
    while (getline(file, line)) {
        if (!line.empty() && SETADD(set, line)) loaded++;
    }
    cout << "Загружено " << loaded << " элементов из файла " << filename << "\n";
    return true;
}

bool save_set_to_file(const Set& set, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка: Не удалось создать файл " << filename << "\n";
        return false;
    }
    int saved = 0;
    for (auto& b : set.buckets) {
        b.forEach([&](const string& v) { file << v << "\n"; saved++; });
    }
    cout << "Сохранено " << saved << " элементов в файл " << filename << "\n";
    return true;
}

void print_help() {
    cout << "Использование:\n";
    cout << "  ./program --file <файл> --query \"<команда>\"\n\n";
    cout << "Команды:\n";
    cout << "  SETADD <значение>    - добавить элемент\n";
    cout << "  SETDEL <значение>    - удалить элемент\n";
    cout << "  SET_AT <значение>    - проверить наличие\n";
    cout << "  PRINT                - вывести все элементы\n";
    cout << "  SAVE                 - сохранить в файл\n";
}

bool parse_arguments(int argc, char* argv[], string& filename, string& query) {
    filename.clear(); query.clear();
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) filename = argv[++i];
        else if (arg == "--query" && i + 1 < argc) query = argv[++i];
        else if (arg == "--help" || arg == "-h") { print_help(); return false; }
    }
    if (filename.empty() || query.empty()) {
        cout << "Ошибка: нужны --file и --query\nСправка: --help\n";
        return false;
    }
    return true;
}

void execute_query(Set& set, const string& query, const string& filename) {
    string command, value;
    size_t pos = query.find(' ');
    if (pos != string::npos) {
        command = query.substr(0, pos);
        value   = query.substr(pos + 1);
    } else command = query;

    if (command == "SETADD") {
        if (value.empty()) { cout << "Ошибка: SETADD требует значение\n"; return; }
        if (SETADD(set, value)) { cout << "Элемент '" << value << "' добавлен\n"; save_set_to_file(set, filename); }
        else cout << "Элемент '" << value << "' уже существует\n";
    }
    else if (command == "SETDEL") {
        if (value.empty()) { cout << "Ошибка: SETDEL требует значение\n"; return; }
        if (SETDEL(set, value)) { cout << "Элемент '" << value << "' удалён\n"; save_set_to_file(set, filename); }
        else cout << "Элемент '" << value << "' не найден\n";
    }
    else if (command == "SET_AT") {
        if (value.empty()) { cout << "Ошибка: SET_AT требует значение\n"; return; }
        if (SET_AT(set, value)) cout << "Элемент '" << value << "' присутствует\n";
        else                    cout << "Элемент '" << value << "' отсутствует\n";
    }
    else if (command == "PRINT") print_set(set);
    else if (command == "SAVE")  save_set_to_file(set, filename);
    else {
        cout << "Ошибка: неизвестная команда '" << command << "'\n";
        cout << "Доступно: SETADD, SETDEL, SET_AT, PRINT, SAVE\n";
    }
}

int main(int argc, char* argv[]) {
    string filename, query;
    if (!parse_arguments(argc, argv, filename, query)) return 1;

    Set mySet;
    if (!load_set_from_file(mySet, filename))
        cout << "Файл не найден — будет создан новый при сохранении\n";

    execute_query(mySet, query, filename);
    return 0;
}