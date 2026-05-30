#pragma once
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

struct Preset {
    std::string name;
    std::string path;
};

struct PresetData {
    // Transform
    float blur_amount    = 0.625f;
    float sharp_amount   = 0.072f;
    int   sample_mode    = 0;
    float x_offset       = 0.0f;
    float y_offset       = 0.0f;
    bool  scale_relative = false;
    int   size_relative_x = 2;
    int   size_relative_y = 2;
    int   size_absolute_x = 256;
    int   size_absolute_y = 256;

    // Color
    float brightness = -0.116f;
    float contrast   =  1.130f;
    float saturation =  2.390f;
    float hue        = 45.0f;
    float gamma      =  0.760f;
    int   tint_red   = 255;
    int   tint_green = 255;
    int   tint_blue  = 255;

    // Dither
    bool  kmeanspp        = true;
    int   alpha_threshold = 30;
    float dither_amount   = 0.462f;
    int   target_colors   = 8;
    int   dither_mode     = 8;
    int   color_dist      = 0;

    nlohmann::json ToJson() const {
        return {
            {"blur_amount",     blur_amount},
            {"sharp_amount",    sharp_amount},
            {"sample_mode",     sample_mode},
            {"x_offset",        x_offset},
            {"y_offset",        y_offset},
            {"scale_relative",  scale_relative},
            {"size_relative_x", size_relative_x},
            {"size_relative_y", size_relative_y},
            {"size_absolute_x", size_absolute_x},
            {"size_absolute_y", size_absolute_y},
            {"brightness",      brightness},
            {"contrast",        contrast},
            {"saturation",      saturation},
            {"hue",             hue},
            {"gamma",           gamma},
            {"tint_red",        tint_red},
            {"tint_green",      tint_green},
            {"tint_blue",       tint_blue},
            {"kmeanspp",        kmeanspp},
            {"alpha_threshold", alpha_threshold},
            {"dither_amount",   dither_amount},
            {"target_colors",   target_colors},
            {"dither_mode",     dither_mode},
            {"color_dist",      color_dist},
        };
    }

    void FromJson(const nlohmann::json& j) {
        auto get = [&](const char* key, auto& field) {
            if (j.contains(key))
                field = j[key].get<std::decay_t<decltype(field)>>();
        };
        get("blur_amount",     blur_amount);
        get("sharp_amount",    sharp_amount);
        get("sample_mode",     sample_mode);
        get("x_offset",        x_offset);
        get("y_offset",        y_offset);
        get("scale_relative",  scale_relative);
        get("size_relative_x", size_relative_x);
        get("size_relative_y", size_relative_y);
        get("size_absolute_x", size_absolute_x);
        get("size_absolute_y", size_absolute_y);
        get("brightness",      brightness);
        get("contrast",        contrast);
        get("saturation",      saturation);
        get("hue",             hue);
        get("gamma",           gamma);
        get("tint_red",        tint_red);
        get("tint_green",      tint_green);
        get("tint_blue",       tint_blue);
        get("kmeanspp",        kmeanspp);
        get("alpha_threshold", alpha_threshold);
        get("dither_amount",   dither_amount);
        get("target_colors",   target_colors);
        get("dither_mode",     dither_mode);
        get("color_dist",      color_dist);
    }

    static PresetData LoadFromFile(const std::string& path) {
        PresetData data;
        std::ifstream f(path);
        if (!f) return data;
        try {
            nlohmann::json j;
            f >> j;
            data.FromJson(j);
        } catch (...) {}
        return data;
    }
};

inline nlohmann::json SerializePreset(const Preset& p) {
    return {{"name", p.name}, {"path", p.path}};
}

inline Preset DeserializePreset(const nlohmann::json& j) {
    return {
        j.value("name", std::string{}),
        j.value("path", std::string{})
    };
}