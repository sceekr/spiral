#pragma once

#include <cstdint>
#include <string>

using namespace std;

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    string as_html();

    void from_html(const char* html);
};