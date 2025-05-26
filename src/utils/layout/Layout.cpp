#include "utils/layout/Layout.hpp"

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