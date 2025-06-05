#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "layout/Layer.hpp"
#include "G4Element.hh"
#include "G4Material.hh"
#include "nlohmann/json.hpp"

using namespace std;

#ifndef LAYER_HPP
#define LAYER_HPP
struct Layer;
struct BoolLayer;
#endif

enum class LayerProcess {
    Keep,
    BoolOp,
    BoolOut,
    Ignore
};

struct ProcessTechnology {
    string name;

    vector<Layer> layers;
    vector<Layer> wafer_layers;
    vector<BoolLayer> bool_layers;

    vector<G4Element*> elements;
    vector<G4Material*> materials;


    G4Element* G4_find_symbol(const string symb);

    void add_json_layers(const nlohmann::json& layers_j, vector<Layer>& target);
    void json_import(const nlohmann::json& json);

    bool layer_exists(const char* search);
    Layer* get_layer(const char* name);
    Layer* get_layer(const uint16_t number, const uint8_t type);

    void G4_json_import(nlohmann::json json);
    nlohmann::json G4_json_export();
};

template <typename G4_type>
G4_type* G4_find_name(vector<G4_type*>& arr, const string& search) {
    auto it = find_if(arr.begin(), arr.end(),
        [&search](G4_type* cur) {
            return cur->GetName() == search;
        });

    if (it != arr.end()) return *it;
    return nullptr;
}