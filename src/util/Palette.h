// Palette.h
#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "imgui.h"

struct PaletteColor {
    int r = 255;
    int g = 255;
    int b = 255;
    int a = 255;
};

inline std::vector<PaletteColor>& GetPalette()
{
    static std::vector<PaletteColor> palette;
    return palette;
}

inline bool LoadPalette(const std::string& path)
{
    auto& palette = GetPalette();

    palette.clear();

    std::ifstream file(path);

    if (!file) {
        std::cout << "[Palette] Failed to open: "
                  << path
                  << "\n";
        return false;
    }

    std::string line;
    int line_index = 0;

    while (std::getline(file, line)) {

        line_index++;

        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        std::stringstream ss(line);

        PaletteColor color;

        ss >> color.r
           >> color.g
           >> color.b;

        if (!(ss >> color.a))
            color.a = 255;

        palette.push_back(color);
    }

    std::cout
        << "[Palette] Loaded "
        << palette.size()
        << " colors\n";

    return true;
}

inline ImU32 Palette(int index)
{
    auto& palette = GetPalette();

    if (index < 0 || index >= (int)palette.size()) {

        std::cout
            << "[Palette] Invalid color index: "
            << index
            << "\n";

        return IM_COL32(255, 0, 255, 255);
    }

    const auto& c = palette[index];

    return IM_COL32(c.r, c.g, c.b, c.a);
}