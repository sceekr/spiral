#pragma once

#include <vector>
#include <algorithm>
#include <map>

#include "gdstk/gdstk.hpp"

using namespace std;

struct Rectangle {
    double xmin, ymin;
    double xmax, ymax;

    Rectangle()
        : xmin(0), ymin(0), xmax(0), ymax(0) {}

    Rectangle(double x_min, double y_min, double x_max, double y_max)
        : xmin(x_min), ymin(y_min), xmax(x_max), ymax(y_max) {}

    double width() const {
        return xmax - xmin;
    }

    double height() const {
        return ymax - ymin;
    }

    double area() const {
        return width() * height();
    }

    bool valid() const {
        return xmax >= xmin && ymax >= ymin;
    }

    void offset(double dx, double dy) {
        xmin += dx;
        xmax += dx;
        ymin += dy;
        ymax += dy;
    }
};

vector<double> get_scanline_intersections(gdstk::Polygon* poly, double y);

gdstk::Array<gdstk::Vec2> single_bounding_box(gdstk::Polygon* poly);

void poly_to_rects_single(gdstk::Polygon* poly, vector<Rectangle>& out);

void poly_to_rects(gdstk::Polygon* poly, vector<Rectangle>& out);