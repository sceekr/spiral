#include <iostream>
#include <CLI/CLI.hpp>

#include "utils/layout/parse.hpp"

using namespace std;

int main(int argc, char** argv) {
    string layout_path;
    gdstk::Library lib;

    CLI::App app{"SPIRAL"};

    app.add_option("-l,--layout", layout_path, "Path to GDS/OASIS file")->required();

    cout << "SPIRAL: SPICE-based IRradiation Analysis for Layouts" << endl;

    CLI11_PARSE(app, argc, argv);
    
    if (! open_lib(lib, layout_path.c_str())) return 1;

    lib.print(true);

    lib.free_all();

    return 0;
}