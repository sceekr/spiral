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
    gdstk::Array<gdstk::Polygon*> polys;
    vector<Rectangle> rects;

    for (uint64_t i = 0; i < lib_.cell_array.count; i++) {
        gdstk::Cell* cell = lib_.cell_array[i];

        for (uint64_t j = 0; j < cell->flexpath_array.count; j++) {
            gdstk::FlexPath* fpath = cell->flexpath_array[j];
            fpath->to_polygons(false, NULL, polys);
        }

        for (uint64_t j = 0; j < cell->robustpath_array.count; j++) {
            gdstk::RobustPath* fpath = cell->robustpath_array[j];
            fpath->to_polygons(false, NULL, polys);
        }

        polys.copy_from(cell->polygon_array);
    }

    for (uint64_t i = 0; i < polys.count; i++) poly_to_rects(polys[i], rects);

    return rects;
}