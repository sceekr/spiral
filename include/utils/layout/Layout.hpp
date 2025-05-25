#pragma once

#include <vector>

#include "gdstk/gdstk.hpp"

#include "utils/other.hpp"
#include "utils/layout/geometry.hpp"

using namespace std;

class Layout {
public:
    Layout();
    ~Layout();

    bool open_layout(const char* path);

    vector<Rectangle> rectangle_decomp();

private:
    gdstk::Library lib_;
};