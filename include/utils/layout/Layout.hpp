#pragma once

#include <vector>
#include <cstdio>

#include "gdstk/gdstk.hpp"

#include "utils/other.hpp"
#include "utils/layout/geometry.hpp"

using namespace std;

class Layout {
public:
    Layout();
    ~Layout();

    bool open_layout(const char* path);
    bool export_used_layers(const char* path);

    vector<Rectangle> rectangle_decomp();
    void parse_layout();

    static bool polygon_layer_compare(gdstk::Polygon* const& a, gdstk::Polygon* const& b);
    static int polygon_layer_getter(gdstk::Polygon* const& a);

private:
    gdstk::Library lib_;
};