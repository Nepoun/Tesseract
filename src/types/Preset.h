#pragma once
#include <string>
#include "nlohmann/json.hpp"

struct Preset {
    char name[128] = "";
    char path[512] = "";
};

inline nlohmann::json SerializePreset(const Preset& p) {
    return {
        { "name", p.name },
        { "path", p.path }
    };
}

inline Preset DeserializePreset(const nlohmann::json& j) {
    Preset p;
    if (j.contains("name"))
        strncpy(p.name, j["name"].get<std::string>().c_str(), sizeof(p.name) - 1);
    if (j.contains("path"))
        strncpy(p.path, j["path"].get<std::string>().c_str(), sizeof(p.path) - 1);
    return p;
}