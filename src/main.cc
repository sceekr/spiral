#include <iostream>

#include "utils/layout/parse.h"

int main(int argc, char** argv) {
    std::cout << "SPIRAL: SPICE-based IRradiation Analysis for Layouts" << std::endl;

    if (argc > 1) {
        std::cout << "Received " << argc - 1 << " argument(s)." << std::endl;
        for (int i = 1; i < argc; ++i) {
            std::cout << "Arg " << i << ": " << argv[i] << std::endl;
        }
    } else {
        std::cout << "No arguments provided." << std::endl;
    }

    // ceci est vide

    return 0;
}