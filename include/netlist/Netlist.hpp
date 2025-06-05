#pragma once

#include <vector>

using namespace std;

class Netlist {
public:
    Netlist();
    ~Netlist();

    bool open_netlist(const char* path);

private:
};