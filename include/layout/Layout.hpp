#pragma once

#include <vector>
#include <cstdio>

#include "layout/utils/geometry.hpp"
#include "layout/Layer.hpp"
#include "layout/ProcessTechnology.hpp"
#include "gdstk/gdstk.hpp"

using namespace std;

#ifndef LAYER_HPP
#define LAYER_HPP
struct Layer;
struct BoolLayer;
#endif

struct LayerRect {
    Rectangle rect;
    Layer* layer;
};

class Layout {
public:
    Layout();
    ~Layout();

    vector<LayerRect> rects;

    bool open_layout(const char* path);
    bool open_techfile(const char* path);
    bool init_techfile(const char* path);

    void process_regular_layers(const gdstk::Array<gdstk::Cell*>& tops);
    void process_boolean_layers(const gdstk::Array<gdstk::Cell*>& tops);
    void process_layers();
    vector<Rectangle> rectangle_decomp();

    static bool polygon_layer_compare(gdstk::Polygon* const& a, gdstk::Polygon* const& b) { return gdstk::get_layer(a->tag) < gdstk::get_layer(b->tag); };
    static int polygon_layer_getter(gdstk::Polygon* const& a) { return gdstk::get_layer(a->tag); };
private:
    gdstk::Library lib_;
    ProcessTechnology tech_;
};
