#include "utils/layout/parse.hpp"

bool open_lib(gdstk::Library& lib, const char* path) {
    gdstk::ErrorCode err = gdstk::ErrorCode::NoError;

    if (ends_with(path, "gds"))
        lib = gdstk::read_gds(path, 0, 1e-2, NULL, &err);
    else if (ends_with(path, "oas"))
        lib = gdstk::read_oas(path, 0, 1e-2, &err);
    else
        return false;

    return err == gdstk::ErrorCode::NoError;
}