#pragma once

#include <string>
#include <vector>
#include <variant>
#include "types/Preset.h"
#include "util/Palette.h"
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
        case PinType::String:  return Palette(39); // verde
        case PinType::Preset:  return Palette(31); // laranja
        case PinType::Int:     return Palette(47); // azul
        case PinType::Float:   return Palette(52); // roxo
        case PinType::Bool:    return Palette(64); // vermelho
        default:               return Palette(7); // cinza
    }
}

inline bool IsColorDark(ImU32 color)
{
    ImVec4 c = ImGui::ColorConvertU32ToFloat4(color);

    float luminance =
        (0.299f * c.x) +
        (0.587f * c.y) +
        (0.114f * c.z);

    return luminance < 0.5f;
}

inline ImU32 GetTextColorForBackground(ImU32 bg)
{
    return IsColorDark(bg)
        ? IM_COL32(255, 255, 255, 255) // branco
        : IM_COL32(0, 0, 0, 255);      // preto
}