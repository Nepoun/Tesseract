#pragma once

#include <string>
#include <vector>
#include <variant>
#include "types/Preset.h"

enum class PinKind {
    Input,
    Output
};

enum class PinType {
    Int,
    Float,
    Bool,
    String,
    Preset
};

using PinValue = std::variant<
    int,
    float,
    bool,
    Preset,
    std::string
>;

struct NodePin {
    int id;
    int node_id;
    std::string name;
    PinType type;


    NodePin(
        int id,
        int node_id,
        const char* name,
        PinType type
    ):
        id(id), 
        node_id(node_id),
        name(name),
        type(type) {}
};

inline ImU32 PinColor(PinType type) {
    switch (type) {
        case PinType::String:  return IM_COL32(94,  214, 114, 255); // verde
        case PinType::Preset:  return IM_COL32(214, 134, 94,  255); // laranja
        case PinType::Int:     return IM_COL32(94,  148, 214, 255); // azul
        case PinType::Float:   return IM_COL32(174, 94,  214, 255); // roxo
        case PinType::Bool:    return IM_COL32(214, 94,  94,  255); // vermelho
        default:               return IM_COL32(180, 180, 180, 255); // cinza
    }
}