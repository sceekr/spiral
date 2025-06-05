#pragma once

#include <string>
#include <vector>

#include "utils/Color.hpp"
#include "layout/utils/geometry.hpp"
#include "nlohmann/json.hpp"
#include "G4Element.hh"
#include "G4Material.hh"

#ifndef PROCESSTECHNOLOGY_HPP
#define PROCESSTECHNOLOGY_HPP
struct ProcessTechnology;
#endif

using namespace std;

enum class Operation {
    Or,
    And,
    Xor,
    Not,
    UNKNOWN
};

struct Layer {
    uint16_t number;
    uint8_t type;

    string name;
    string desc;
    Color color;

    G4Element* element;
    G4Material* material;
    
    uint8_t level;
    float offset;
    float thickness;

    bool ignore;

    vector<Rectangle> geometry;


    virtual void json_import(const nlohmann::json& json, ProcessTechnology& tech);
    nlohmann::json json_export();
};

struct BoolLayer : Layer {
    Layer* operand0;
    Operation operation;
    Layer* operand1;

    void json_import(const nlohmann::json& json, ProcessTechnology& tech) override;
};

Operation parse_operation(const char* op);
