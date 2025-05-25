#include "utils/layout/geometry.hpp"

vector<double> get_scanline_intersections(gdstk::Polygon* poly, double y) {
    vector<double> inters;
    gdstk::Array<gdstk::Vec2>& verts = poly->point_array;
    uint32_t cnt = verts.count;

    for (int i = 0; i < cnt; ++i) {
        gdstk::Vec2 vert1 = verts[i];
        gdstk::Vec2 vert2 = verts[(i + 1) % cnt];

        if ((vert1.y <= y && vert2.y > y) || (vert2.y <= y && vert1.y > y)) {
            double x = vert1.x + (y - vert1.y) * (vert2.x - vert1.x) / (vert2.y - vert1.y);
            inters.push_back(x);
        }
    }

    sort(inters.begin(), inters.end());
    return inters;
}

void poly_to_rects(gdstk::Polygon* poly, vector<Rectangle>& out) {
    if (! poly->point_array.count) return;

    vector<double> y_values;
    for (uint64_t i = 0; i < poly->point_array.count; i++) {
        y_values.push_back(poly->point_array[i].y);
    }
    sort(y_values.begin(), y_values.end());
    y_values.erase(unique(y_values.begin(), y_values.end()), y_values.end());

    map<pair<double, double>, Rectangle> active_rects;

    for (uint64_t i = 0; i + 1 < y_values.size(); ++i) {
        double y1 = y_values[i];
        double y2 = y_values[i + 1];
        double ymin = y1, ymax = y2;

        if (ymin > ymax) swap(ymin, ymax);

        vector<double> x_inters = get_scanline_intersections(poly, (ymin + ymax) / 2.0);

        for (size_t j = 0; j + 1 < x_inters.size(); j += 2) {
            double x_start = x_inters[j];
            double x_end = x_inters[j + 1];

            pair<double, double> key = {x_start, x_end};

            if (active_rects.count(key)) {
                active_rects[key].ymax = ymax;
            } else {
                Rectangle r = {x_start, x_end, ymin, ymax};
                active_rects[key] = r;
            }
        }

        vector<std::pair<double, double>> to_remove;
        for (const auto& [key, rect] : active_rects) {
            if (find(x_inters.begin(), x_inters.end(), key.first) == x_inters.end()) {
                out.push_back(rect);
                to_remove.push_back(key);
            }
        }

        for (const auto& key : to_remove) {
            active_rects.erase(key);
        }
    }

    for (const auto& [key, rect] : active_rects) {
        out.push_back(rect);
    }
}