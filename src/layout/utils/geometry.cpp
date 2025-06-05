#include "layout/utils/geometry.hpp"

vector<double> get_scanline_intersections(gdstk::Polygon* poly, double y) {
    vector<double> inters;
    gdstk::Array<gdstk::Vec2>& verts = poly->point_array;
    uint32_t cnt = verts.count;

    for (int i = 0; i < cnt; ++i) {
        gdstk::Vec2 vert1 = verts[i];
        gdstk::Vec2 vert2 = verts[(i + 1) % cnt];

        if (vert1.y == vert2.y) continue;

        if ((vert1.y <= y && vert2.y > y) || (vert2.y <= y && vert1.y > y)) {
            double x = vert1.x + (y - vert1.y) * (vert2.x - vert1.x) / (vert2.y - vert1.y);
            inters.push_back(x);
        }
    }

    sort(inters.begin(), inters.end());
    return inters;
}

gdstk::Array<gdstk::Vec2> single_bounding_box(gdstk::Polygon* poly) {
    gdstk::Vec2* cur = &poly->point_array[0];
    Rectangle box = Rectangle(cur->x, cur->y, cur->x, cur->y);
    gdstk::Array<gdstk::Vec2> out = {};

    for (uint8_t i = 1; i < 3; ++i) {
        cur = &poly->point_array[i];
        if (cur->x < box.xmin) box.xmin = cur->x;
        if (cur->x > box.xmax) box.xmax = cur->x;
        if (cur->y < box.ymin) box.ymin = cur->y;
        if (cur->y > box.ymax) box.ymax = cur->y;
    }

    out.append({box.xmin, box.ymin});
    out.append({box.xmax, box.ymin});
    out.append({box.xmax, box.ymax});
    out.append({box.xmin, box.ymax});

    return out;
}

void poly_to_rects_single(gdstk::Polygon* poly, vector<Rectangle>& out) {
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

        if (ymin == ymax) continue;
        if (ymin > ymax) swap(ymin, ymax);

        vector<double> x_inters = get_scanline_intersections(poly, (ymin + ymax) / 2.0);

        for (size_t j = 0; j + 1 < x_inters.size(); j += 2) {
            double xmin = x_inters[j];
            double xmax = x_inters[j + 1];

            if (xmin == xmax) continue;

            pair<double, double> key = {xmin, xmax};

            if (active_rects.count(key)) {
                active_rects[key].ymax = ymax;
            } else {
                Rectangle r = Rectangle(xmin, ymin, xmax, ymax);
                active_rects[key] = r;
            }
        }

        vector<pair<double, double>> rem;
        for (const auto& [key, rect] : active_rects) {
            if (find(x_inters.begin(), x_inters.end(), key.first) == x_inters.end()) {
                out.push_back(rect);
                rem.push_back(key);
            }
        }

        for (const auto& key : rem) {
            active_rects.erase(key);
        }
    }

    for (const auto& [key, rect] : active_rects) {
        out.push_back(rect);
    }
}

void poly_to_rects(gdstk::Polygon* poly, vector<Rectangle>& out) {
    if (poly->point_array.count < 3) return;

    if (poly->point_array.count == 3) {
        gdstk::Array<gdstk::Vec2> box = single_bounding_box(poly);
        poly->point_array.clear();
        poly->point_array.extend(box);
        box.clear();
    }

    uint64_t start_idx = out.size();
    poly_to_rects_single(poly, out);
    uint64_t end_idx = out.size();
    uint64_t size = end_idx - start_idx;

    if (poly->repetition.type != gdstk::RepetitionType::None) {
        if (! size) return;

        gdstk::Array<gdstk::Vec2> offsets = {};
        poly->repetition.get_offsets(offsets);

        for (uint64_t i = 0; i < offsets.count; i++) {
            gdstk::Vec2 offset = offsets[i];

            out.insert(out.end(), out.begin() + start_idx, out.begin() + end_idx);

            for_each(out.end() - size, out.end(), [offset](Rectangle& rect) {
                rect.offset(offset.x, offset.y);
            });
        }
        
        offsets.clear();
    }
}