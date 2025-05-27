#pragma once

#include <cstring>
#include <cctype>
#include <vector>
#include <algorithm>

#include "nlohmann/json.hpp"
#include "gdstk/gdstk.hpp"

using namespace std;

bool ends_with(const char* str, const char* search);

bool save_json(const nlohmann::json& j, const char* path);

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