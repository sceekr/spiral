#include "layout/Layout.hpp"

#include <iostream>
#include <string>

#include "gdstk/sort.hpp"
#include "utils/other.hpp"
#include "layout/utils/geometry.hpp"

Layout::Layout() : lib_() {}

Layout::~Layout() {}

bool Layout::open_layout(const char* path) {
    gdstk::ErrorCode err = gdstk::ErrorCode::NoError;

    if (ends_with(path, "gds"))
        lib_ = gdstk::read_gds(path, 0, 1e-2, NULL, &err);
    else if (ends_with(path, "oas"))
        lib_ = gdstk::read_oas(path, 0, 1e-2, &err);
    else
        return false;

    return err == gdstk::ErrorCode::NoError;
}

/*
void Layout::process_layers() {
    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};
    gdstk::Array<gdstk::Polygon*> polys = {};

    lib_.top_level(tops, raw);
    raw.clear();

    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Array<gdstk::Polygon*> polys = {};

        gdstk::Cell* cell = tops[i];

        cell->get_polygons(false, true, -1, false, 0, polys);

        for (uint64_t j = 0; j < polys.count; j++) {
            gdstk::Polygon* poly = polys[j];
            vector<Rectangle> vrects;

            uint16_t number = gdstk::get_layer(poly->tag);
            uint8_t type = gdstk::get_type(poly->tag);

            Layer* layer = tech_.get_layer(number, type);

            if (BoolLayer* check = dynamic_cast<BoolLayer*>(layer)) continue;   // NORMALLY not possible...
            if (layer->ignore) continue;

            poly_to_rects(poly, vrects);

            for (size_t k = 0; k < vrects.size(); k++) rects.push_back((LayerRect) {vrects[k], layer});
            vrects.clear();
        } // CACA PROUT #define proutnoe
    }

    for (size_t i = 0; i < tech_.bool_layers.size(); i++) {
        BoolLayer* blayer = &tech_.bool_layers[i];
        gdstk::Array<gdstk::Polygon*> polys = {};

        for (uint64_t j = 0; j < tops.count; j++) {
            gdstk::Cell* cell = tops[i];
            gdstk::Array<gdstk::Polygon*> operand0 = {};

            cell->get_polygons(false, true, -1, true, gdstk::make_tag(blayer->operand0->number, blayer->operand0->type), operand0);

            for (uint64_t k = 0; k < operand0.count; k++) {
                gdstk::Array<gdstk::Polygon*> operand1 = {};

                cell->get_polygons(false, true, -1, true, gdstk::make_tag(blayer->operand1->number, blayer->operand1->type), operand1);

                for (uint64_t l = 0; l < operand1.count; l++) gdstk::boolean(*operand0[k], *operand1[l], static_cast<gdstk::Operation>(blayer->operation), 1.0, polys);
            }
        }

        for (uint64_t j = 0; j < polys.count; j++) {
            gdstk::Polygon* poly = polys[j];
            vector<Rectangle> vrects;

            poly_to_rects(poly, vrects);


            for (size_t k = 0; k < vrects.size(); k++) rects.push_back((LayerRect) {vrects[k], blayer});
            vrects.clear();
        }
    } // aux secours ceci n'est pas tres efficient
}
*/

void Layout::process_layers() {
    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};

    lib_.top_level(tops, raw);
    raw.clear();

    rects.clear();

    process_regular_layers(tops);
    
    process_boolean_layers(tops);
    
    tops.clear();
}

void Layout::process_regular_layers(const gdstk::Array<gdstk::Cell*>& tops) {
    gdstk::Array<gdstk::Polygon*> all_polys = {};
    
    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Cell* cell = tops[i];
        cell->get_polygons(false, true, -1, false, 0, all_polys);
    }

    for (uint64_t i = 0; i < all_polys.count; i++) {
        gdstk::Polygon* poly = all_polys[i];
        
        uint16_t number = gdstk::get_layer(poly->tag);
        uint8_t type = gdstk::get_type(poly->tag);

        Layer* layer = tech_.get_layer(number, type);
        
        if (!layer || dynamic_cast<BoolLayer*>(layer) || layer->ignore) continue;

        vector<Rectangle> vrects;
        poly_to_rects(poly, vrects);
        
        for (const Rectangle& rect : vrects) rects.push_back({rect, layer});
    }
    
    all_polys.clear();
}

void Layout::process_boolean_layers(const gdstk::Array<gdstk::Cell*>& tops) {
    for (size_t i = 0; i < tech_.bool_layers.size(); i++) {
        BoolLayer* blayer = &tech_.bool_layers[i];
        
        gdstk::Array<gdstk::Polygon*> operand0_polys = {};
        gdstk::Array<gdstk::Polygon*> operand1_polys = {};
        
        for (uint64_t j = 0; j < tops.count; j++) {
            gdstk::Cell* cell = tops[j];
            
            cell->get_polygons(false, true, -1, true, 
                gdstk::make_tag(blayer->operand0->number, blayer->operand0->type), 
                operand0_polys);
                
            cell->get_polygons(false, true, -1, true, 
                gdstk::make_tag(blayer->operand1->number, blayer->operand1->type), 
                operand1_polys);
        }
        
        gdstk::Array<gdstk::Polygon*> result_polys = {};
        
        for (uint64_t k = 0; k < operand0_polys.count; k++) {
            for (uint64_t l = 0; l < operand1_polys.count; l++) {
                gdstk::boolean(*operand0_polys[k], *operand1_polys[l], 
                    static_cast<gdstk::Operation>(blayer->operation), 1.0, result_polys);
            }
        }
        
        for (uint64_t j = 0; j < result_polys.count; j++) {
            gdstk::Polygon* poly = result_polys[j];
            vector<Rectangle> vrects;
            
            poly_to_rects(poly, vrects);
            
            for (const Rectangle& rect : vrects) {
                rects.push_back({rect, blayer});
            }
        }
        
        operand0_polys.clear();
        operand1_polys.clear();
        result_polys.clear();
    }
}

vector<Rectangle> Layout::rectangle_decomp() {
    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};
    gdstk::Array<gdstk::Polygon*> polys = {};
    vector<Rectangle> rects;

    lib_.top_level(tops, raw);
    raw.clear();

    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Cell* cell = tops[i];

        cell->get_polygons(false, true, -1, false, 0, polys);
    }

    for (uint64_t i = 0; i < polys.count; i++) poly_to_rects(polys[i], rects);

    polys.clear();

    return rects;
}

bool Layout::init_techfile(const char* path) {
    if (! path || ! path[0]) return false;

    gdstk::Array<gdstk::Cell*> tops = {};
    gdstk::Array<gdstk::RawCell*> raw = {};
    gdstk::Array<gdstk::Polygon*> polys = {};

    lib_.top_level(tops, raw);
    raw.clear();

    if (! tops.count) return false;

    for (uint64_t i = 0; i < tops.count; i++) {
        gdstk::Cell* cell = tops[i];

        cell->get_polygons(false, true, -1, false, 0, polys);
    }

    if (! polys.count) return false;

    gdstk_array_unique<gdstk::Polygon*,int>(polys, &polygon_layer_getter);

    gdstk::sort(polys, &Layout::polygon_layer_compare);

    nlohmann::json out;

    for (uint64_t i = 0; i < polys.count; i++) {
        gdstk::Polygon* poly = polys[i];

        gdstk::get_layer(poly->tag);

        nlohmann::json layer;
        layer["name"] = "Layer_" + to_string(i);
        layer["level"] = gdstk::get_layer(poly->tag);
        layer["type"] = gdstk::get_type(poly->tag);
        layer["thickness"] = 0;
        layer["material_name"] = "";
        layer["element_name"] = "";

        out["layers"].push_back(layer);
    }

    return save_json(out, path);
}

bool Layout::open_techfile(const char* path) {
    if (! ends_with(path, "json")) return false;

    nlohmann::json techfile;
    load_json(techfile, path);

    tech_ = ProcessTechnology();
    tech_.json_import(techfile);
    
    return true;
}