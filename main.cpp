#include <iostream>

#include "utils/layout/parse.hpp"

int main(int argc, char** argv) {
    std::cout << "SPIRAL: SPICE-based IRradiation Analysis for Layouts" << std::endl;

    if (argc == 2) {
        gdstk::Library lib;
        if (! open_lib(lib, argv[1])) return 1;

        lib.print(true);

        lib.free_all();
    } else {
        std::cout << "Expected one argument, the GDS/OAS path." << std::endl;
    }

    return 0;
}