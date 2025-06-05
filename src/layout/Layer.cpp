#include "layout/Layer.hpp"
#include "layout/ProcessTechnology.hpp"
#include "utils/other.hpp"

Operation parse_operation(const char* op) {
    if (equals(op, "or")) return Operation::Or;
    if (equals(op, "and")) return Operation::And;
    if (equals(op, "xor")) return Operation::Xor;
    if (equals(op, "not")) return Operation::Not;
    return Operation::UNKNOWN;
}

nlohmann::json Layer::json_export() {
    nlohmann::json layer;

    layer["number"] = number;
    layer["datatype"] = type;
    layer["name"] = name;
    layer["desc"] = desc;
    layer["color"] = color.as_html();
    layer["element"] = (element) ? element->GetName().c_str() : "";
    layer["material"] = (material) ? material->GetName().c_str() : "";
    layer["level"] = level;
    layer["offset"] = offset;
    layer["thickness"] = thickness;
    layer["ignore"] = ignore;

    return layer;
}

void Layer::json_import(const nlohmann::json& json, ProcessTechnology& tech) {
    if (json.contains("number"))    number = json["number"].get<int>();
    if (json.contains("datatype"))  type = json["datatype"].get<int>();
    if (json.contains("name"))      name = json["name"].get<string>();
    if (json.contains("desc"))      desc = json["desc"].get<string>();
    if (json.contains("color"))     color.from_html(json["color"].get<string>().c_str());

    if (json.contains("element")) {
        string elem = json["element"].get<string>();
        element = (!elem.empty()) ? G4_find_name(tech.elements, elem) : nullptr;
    }

    if (json.contains("material")) {
        string mat = json["material"].get<string>();
        material = (!mat.empty()) ? G4_find_name(tech.materials, mat) : nullptr;
    }

    if (json.contains("level"))     level = json["level"].get<int>();
    if (json.contains("offset"))    offset = json["offset"].get<double>();
    if (json.contains("thickness")) thickness = json["thickness"].get<double>();
    if (json.contains("ignore"))    ignore = json["ignore"].get<bool>();
}

void BoolLayer::json_import(const nlohmann::json& json, ProcessTechnology& tech) {
    if (! json.contains("output") || ! json.contains("layer_0_name") || ! json.contains("layer_1_name") || ! json.contains("operator")) return;

    const char* name_0 = json["layer_0_name"].get<string>().c_str();
    const char* name_1 = json["layer_1_name"].get<string>().c_str();

    if (! tech.layer_exists(name_0)) return;
    if (! tech.layer_exists(name_1)) return;

    Layer::json_import(json["output"], tech);

    Operation op = parse_operation(json["operator"].get<string>().c_str());

    if (op == Operation::UNKNOWN) return;

    operand0 = tech.get_layer(name_0);
    operand1 = tech.get_layer(name_1);
    operation = op;

    if (json["delete_sources"].get<bool>()) {
        operand0->ignore = true;
        operand1->ignore = true;
    }
}