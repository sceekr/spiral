#include "utils/layout/Layout.hpp"

#include <iostream>
#include "gdstk/sort.hpp"

Layout::Layout() : lib_() {
}

Layout::~Layout() {
}

bool Layout::open_layout(const char* path) {
    gdstk::ErrorCode err = gdstk::ErrorCode::NoError;

    if (ends_with(path, "gds"))
        lib_ = gdstk::read_gds(path, 0, 1e-2, NULL, &err);
    else if (ends_with(path, "oas"))
        lib_ = gdstk::read_oas(path, 0, 1e-2, &err);
    else
        return false;

    return err == gdstk::ErrorCode::NoError;
}

vector<Rectangle> Layout::rectangle_decomp() {
    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};
    gdstk::Array<gdstk::Polygon*> polys = {};
    vector<Rectangle> rects;

    lib_.top_level(tops, raw);
    raw.clear();

    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Cell* cell = tops[i];

        cell->get_polygons(false, true, -1, false, 0, polys);
    }

    for (uint64_t i = 0; i < polys.count; i++) poly_to_rects(polys[i], rects);

    polys.clear();

    return rects;
}

bool Layout::polygon_layer_compare(gdstk::Polygon* const& a, gdstk::Polygon* const& b) {
    return gdstk::get_layer(a->tag) < gdstk::get_layer(b->tag);
}

int Layout::polygon_layer_getter(gdstk::Polygon* const& a) {
    return gdstk::get_layer(a->tag);
}

bool Layout::export_used_layers(const char* path) {
    if (! path || ! path[0]) return false;

    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};
    gdstk::Array<gdstk::Polygon*> polys = {};

    lib_.top_level(tops, raw);
    raw.clear();

    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Cell* cell = tops[i];

        cell->get_polygons(false, true, -1, false, 0, polys);
    }

    if (! polys.count) return false;

    gdstk_array_unique<gdstk::Polygon*,int>(polys, &polygon_layer_getter);

    gdstk::sort(polys, &Layout::polygon_layer_compare);

    nlohmann::json out;

    for (uint64_t i = 0; i < polys.count; i++) {
        gdstk::Polygon* poly = polys[i];
        
        gdstk::get_layer(poly->tag);

        nlohmann::json layer;
        layer["name"] = "Layer_" + std::to_string(i);
        layer["level"] = gdstk::get_layer(poly->tag);
        layer["thickness"] = 0;
        layer["material_name"] = "";
        layer["element_name"] = "";

        out["layers"].push_back(layer);
    }

    return save_json(out, path);
}
