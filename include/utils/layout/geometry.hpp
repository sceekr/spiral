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
};

vector<double> get_scanline_intersections(gdstk::Polygon* poly, double y);

void poly_to_rects(gdstk::Polygon* poly, vector<Rectangle>& out);