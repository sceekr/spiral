#include "layout/ProcessTechnology.hpp"

#include "G4NistManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

G4Element* ProcessTechnology::G4_find_symbol(const string symb) {
    auto it = find_if(elements.begin(), elements.end(),
        [&symb](G4Element* cur) {
            return cur->GetName() == symb;
        });

    if (it != elements.end()) return *it;
    return nullptr;
}

void ProcessTechnology::add_json_layers(const nlohmann::json& layers_j, vector<Layer>& target) {
    for (nlohmann::json::const_iterator it = layers_j.begin(); it != layers_j.end(); ++it) {
        const nlohmann::json& layer_j = *it;           

        string name = layer_j.value("name", "");

        if (this->layer_exists(name.c_str())) continue;

        Layer layer;
        layer.json_import(layer_j, *this);

        target.push_back(layer);
    }
}

void ProcessTechnology::json_import(const nlohmann::json& json) {
    if (json.contains("name"))    name = json["name"].get<string>();

    if (json.contains("layers") && json["layers"].is_array())
        this->add_json_layers(json["layers"], layers);

    if (json.contains("wafer_layers") && json["wafer_layers"].is_array())
        this->add_json_layers(json["wafer_layers"], wafer_layers);

    if (json.contains("boolean_layers") && json["boolean_layers"].is_array()) {
        const nlohmann::json& layers_j = json["boolean_layers"];

        for (nlohmann::json::const_iterator it = layers_j.begin(); it != layers_j.end(); ++it) {
            const nlohmann::json& bool_j = *it;

            if (json.contains("output")) continue;

            const nlohmann::json& output_j = json["output"];

            const char* name = output_j.value("name", "").c_str();

            if (this->layer_exists(name)) continue;

            BoolLayer layer;
            layer.json_import(bool_j, *this);

            bool_layers.push_back(layer);
        }
    }
        
    this->G4_json_import(json);
}

bool ProcessTechnology::layer_exists(const char* search) {
    bool exists = find_if(layers.begin(), layers.end(),
            [&search](const Layer& cur) {
            return cur.name == search;
        }) == layers.end();

    return exists;
}

Layer* ProcessTechnology::get_layer(const char* name) {
    const auto& it = find_if(layers.begin(), layers.end(),
        [&name](const Layer& cur) {
        return cur.name == name;
    });

    if (it == layers.end()) return nullptr;

    return &(*it);
}

Layer* ProcessTechnology::get_layer(const uint16_t number, const uint8_t type) {
    const auto& it = find_if(layers.begin(), layers.end(),
        [&number, &type](const Layer& cur) {
        return (cur.number == number && cur.type == type);
    });

    if (it == layers.end()) return nullptr;

    return &(*it);
}

void ProcessTechnology::G4_json_import(nlohmann::json json) {
    G4NistManager* man = G4NistManager::Instance();

    if (json.contains("elements") && json["elements"].is_array()) {
        const nlohmann::json& elems_j = json["elements"];

        for (nlohmann::json::const_iterator it = elems_j.begin(); it != elems_j.end(); ++it) {
            const nlohmann::json& elem_j = *it;

            if (! elem_j.contains("symbol")) continue;

            const string symbol = elem_j["symbol"];

            G4Element* element = man->FindOrBuildElement(symbol);

            if (! element) {
                const string name = elem_j["name"];
                int atomicNumber = elem_j["atomic_number"];
                double atomicMass = elem_j["atomic_mass"];
                element = new G4Element(name, symbol, atomicNumber, atomicMass * g/mole);
            }

            elements.push_back(element);
        }
    }

    if (json.contains("materials") && json["materials"].is_array()) {
        const nlohmann::json& mats_j = json["materials"];

        for (nlohmann::json::const_iterator it = mats_j.begin(); it != mats_j.end(); ++it) {
            const nlohmann::json& mat_j = *it;

            if (! mat_j.contains("name")) continue;
            const string name = mat_j["name"];

            G4Material* material = man->FindOrBuildMaterial(name, false);
            if (material) {
                materials.push_back(material);
                continue;
            }

            double density = mat_j["density_g_per_cm3"].get<double>() * g/cm3;

            G4State state = kStateUndefined;
            if (mat_j.contains("state")) {
                string stateStr = mat_j["state"];
                if (stateStr == "solid") state = kStateSolid;
                else if (stateStr == "liquid") state = kStateLiquid;
                else if (stateStr == "gas") state = kStateGas;
            }

            double temperature = (mat_j.contains("temperature_K")) ? mat_j["temperature_K"].get<double>() * kelvin : CLHEP::STP_Temperature;
            double pressure = (mat_j.contains("pressure_Pa")) ? mat_j["pressure_Pa"].get<double>() * pascal : CLHEP::STP_Pressure;

            int cnt = mat_j["composition"]["elements"].size();

            if (state != kStateUndefined) {
                material = new G4Material(name, density, cnt, state, temperature, pressure);
            } else {
                material = new G4Material(name, density, cnt);
            }

            const nlohmann::json& comp = mat_j["composition"];
            string type = comp["type"];

            const nlohmann::json& mat_elems = comp["elements"];

            for (nlohmann::json::const_iterator it = mat_elems.begin(); it != mat_elems.end(); ++it) {
                const nlohmann::json& elem = *it;

                string symbol = elem["symbol"];
                G4Element* element = G4_find_symbol(symbol);

                if (element == nullptr) {
                    element = man->FindOrBuildElement(symbol, false);
                    if (!element) throw runtime_error("Element \"" + symbol + "\" not found");

                    elements.push_back(element);
                }

                if (type == "fraction") {
                    double fraction = elem["fraction"];
                    material->AddElement(element, fraction);
                } else if (type == "atom_count") {
                    int atoms = elem["atoms"];
                    material->AddElement(element, atoms);
                } else throw runtime_error("Unknown composition type: " + type);
            }

            materials.push_back(material);
        }
    }
}

nlohmann::json ProcessTechnology::G4_json_export() {
    nlohmann::json json;

    json["elements"] = nlohmann::json::array();
    for (const G4Element* element : elements) {
        nlohmann::json cur;
        cur["name"] = element->GetName();
        cur["symbol"] = element->GetSymbol();
        cur["atomic_number"] = element->GetZ();
        cur["atomic_mass"] = element->GetA() / (g/mole);
        json["elements"].push_back(cur);
    }

    json["materials"] = nlohmann::json::array();
    for (const G4Material* material : materials) {
        nlohmann::json cur;

        cur["name"] = material->GetName();
        cur["density"] = material->GetDensity() / (g/cm3);

        switch (material->GetState()) {
            case kStateSolid: cur["state"] = "solid"; break;
            case kStateLiquid: cur["state"] = "liquid"; break;
            case kStateGas: cur["state"] = "gas"; break;
            default: break;
        }

        if (material->GetTemperature() != CLHEP::STP_Temperature) cur["temperature_K"] = material->GetTemperature() / kelvin;
        if (material->GetPressure() != CLHEP::STP_Pressure) cur["pressure_Pa"] = material->GetPressure() / pascal;

        nlohmann::json composition;
        composition["elements"] = nlohmann::json::array();
        composition["type"] = "fraction";

        int cnt = material->GetNumberOfElements();
        for (int i = 0; i < cnt; ++i) {
            double fraction = material->GetFractionVector()[i];

            nlohmann::json sub;
            sub["symbol"] = material->GetElement(i)->GetSymbol();
            sub["fraction"] = fraction;
            composition["elements"].push_back(sub);
        }

        cur["composition"] = composition;
        json["materials"].push_back(cur);
    }

    return json;
}