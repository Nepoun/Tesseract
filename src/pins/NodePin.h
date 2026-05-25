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
