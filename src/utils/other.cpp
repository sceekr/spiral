#include "utils/other.hpp"
#include <cstdio>

bool ends_with(const char* str, const char* search) {
    if (! str || ! search) return false;

    size_t len_str = strlen(str);
    size_t len_search = strlen(search);

    if (len_search > len_str) return false;

    const char* start = str + len_str - len_search;

    for (size_t i = 0; i < len_search; ++i) {
        if (tolower(static_cast<unsigned char>(start[i])) !=
            tolower(static_cast<unsigned char>(search[i]))) {
            return false;
        }
    }
    
    return true;
}

bool equals(const char* a, const char* b) {
    if (! a || ! b) return false;

    while (*a && *b) {
        if (tolower(static_cast<unsigned char>(*a)) !=
            tolower(static_cast<unsigned char>(*b))) {
            return false;
        }
        ++a;
        ++b;
    }

    return *a == *b;
}

char nibble_to_hex(uint8_t nibble) {
    return (nibble < 10) ? ('0' + nibble) : ('a' + (nibble - 10));
}

uint8_t hex_to_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    else if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    else if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return 0;
}

bool save_json(const nlohmann::json& j, const char* path) {
    if (! path || ! path[0]) return false;

    FILE* file = fopen(path, "w");
    if (! file) return false;

    bool saved = (fputs(j.dump(4).c_str(), file) != EOF);

    fclose(file);

    return saved;
}

bool load_json(nlohmann::json& j, const char* path) {
    if (! path || ! path[0]) return false;

    ifstream file(path);
    if (! file.is_open()) return false;

    try {
        file >> j;
        return true;
    } catch (const nlohmann::json::parse_error&) {
        return false;
    }
}
