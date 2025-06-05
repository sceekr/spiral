#pragma once

#include <cstring>
#include <cctype>
#include <vector>
#include <algorithm>
#include <fstream>

#include "nlohmann/json.hpp"
#include "gdstk/gdstk.hpp"

using namespace std;

bool ends_with(const char* str, const char* search);
bool equals(const char* a, const char* b);

char nibble_to_hex(uint8_t nibble);
uint8_t hex_to_nibble(char c);

bool save_json(const nlohmann::json& j, const char* path);
bool load_json(nlohmann::json& j, const char* path) ;

template<typename T, typename C>
void gdstk_array_unique(gdstk::Array<T>& array, C (*getter)(const T&)) {
    vector<C> unique;

    for (uint64_t i = 0; i < array.count;) {
        if (find(unique.begin(), unique.end(), getter(array[i])) != unique.end())
            array.remove(i);
        else
            unique.push_back(getter(array[i++]));
    }

    unique.clear();
}