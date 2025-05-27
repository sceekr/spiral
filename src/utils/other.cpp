#include "utils/other.hpp"

bool ends_with(const char* str, const char* search) {
    size_t len_str = std::strlen(str);
    size_t len_search = std::strlen(search);

    if (len_search > len_str) return false;

    const char* start = str + len_str - len_search;

    for (size_t i = 0; i < len_search; ++i) {
        if (std::tolower(static_cast<unsigned char>(start[i])) !=
            std::tolower(static_cast<unsigned char>(search[i]))) {
            return false;
        }
    }
    return true;
}

bool save_json(const nlohmann::json& j, const char* path) {
    if (! path[0]) return false;

    FILE* file = std::fopen(path, "w");
    if (! file) return false;

    bool saved = (fputs(j.dump(4).c_str(), file) != EOF);

    fclose(file);

    return saved;
}