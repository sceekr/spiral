#include <iostream>
#include <CLI/CLI.hpp>

#include "spiral.hpp"

using namespace std;

int main(int argc, char** argv) {
    string layout_path;
    Layout layout;
    vector<Rectangle> rects;

    CLI::App app{"SPIRAL"};

    app.add_option("-l,--layout", layout_path, "Path to GDS/OASIS file")->required();

    cout << "SPIRAL: SPICE-based IRradiation Analysis for Layouts" << endl;

    CLI11_PARSE(app, argc, argv);
    
    if (! layout.open_layout(layout_path.c_str())) return 1;

    rects = layout.rectangle_decomp();
    for (const Rectangle& rect : rects) {
        cout << "(" << rect.xmin << ", " << rect.ymin << ") (" << rect.xmax << ", " << rect.ymax << ")" <<  endl;
    }

    layout.init_techfile("layers.json");

    return 0;
}