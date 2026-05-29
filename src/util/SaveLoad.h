#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "nodes/NodeBase.h"
#include "registry/NodeRegistry.h"
#include "registry/RuntimeNodeRegistry.h"
#include "util/IdUtil.h"
#include <iostream>

inline void SaveGraph(
    const std::vector<std::unique_ptr<NodeBase>>& nodes,
    const std::string& path)
{
    nlohmann::json j;

    for (const auto& node : nodes)
        j["nodes"].push_back(node->Serialize());

    for (const auto& [id, link] : GetRuntimeNodeRegistry().links) {
        j["links"].push_back({
            { "id",    link.id        },
            { "start", link.pin_start },
            { "end",   link.pin_end   }
        });
    }

    std::ofstream file(path);

    if (!file) {
        std::cout << "[Save] Failed to open file\n";
        return;
    }

    file << j.dump(2);

    std::cout << "[Save] "
              << nodes.size()
              << " nodes, "
              << GetRuntimeNodeRegistry().links.size()
              << " links\n";
}

inline std::vector<Link> g_pending_links;

inline void LoadGraph(
    std::vector<std::unique_ptr<NodeBase>>& nodes,
    const std::string& path)
{
    std::ifstream file(path);

    if (!file) {
        std::cout << "[Load] Failed to open file\n";
        return;
    }

    nlohmann::json j;

    try {
        j = nlohmann::json::parse(file);
    }
    catch (const std::exception& e) {
        std::cout << "[Load] JSON parse error: "
                  << e.what()
                  << "\n";
        return;
    }

    auto& runtime = GetRuntimeNodeRegistry();

    runtime.links.clear();

    for (const auto& node : nodes)
        runtime.RemoveNode(node->id);

    nodes.clear();

    std::unordered_map<int, int> id_map;

    if (!j.contains("nodes")) {
        std::cout << "[Load] Missing nodes array\n";
        return;
    }

    int loaded_nodes = 0;

    for (const auto& jnode : j["nodes"]) {

        if (!jnode.contains("type_id")) {
            std::cout << "[Load] Node missing type_id\n";
            continue;
        }

        std::string type = jnode["type_id"].get<std::string>();

        bool found = false;

        for (auto& entry : GetNodeRegistry()) {

            if (entry.type_id != type)
                continue;

            found = true;

            auto node = entry.create(NextID);

            if (jnode.contains("id"))
                id_map[jnode["id"].get<int>()] = node->id;

            if (jnode.contains("input_ids")) {

                auto old_ids = jnode["input_ids"].get<std::vector<int>>();

                for (size_t i = 0; i < old_ids.size() && i < node->inputs.size(); i++)
                    id_map[old_ids[i]] = node->inputs[i].id;
            }

            if (jnode.contains("output_ids")) {

                auto old_ids = jnode["output_ids"].get<std::vector<int>>();

                for (size_t i = 0; i < old_ids.size() && i < node->outputs.size(); i++)
                    id_map[old_ids[i]] = node->outputs[i].id;
            }

            node->Deserialize(jnode);

            if (jnode.contains("pos_x") && jnode.contains("pos_y")) {
                node->pos = {
                    jnode["pos_x"].get<float>(),
                    jnode["pos_y"].get<float>()
                };
            }

            std::cout << "[Load] Node: "
                      << node->title
                      << "\n";

            nodes.push_back(std::move(node));
            runtime.RegisterNode(nodes.back().get());

            loaded_nodes++;

            break;
        }

        if (!found) {
            std::cout << "[Load] Unknown node type: "
                      << type
                      << "\n";
        }
    }

    g_pending_links.clear();

    int loaded_links = 0;

    if (j.contains("links")) {

        for (const auto& jlink : j["links"]) {

            if (!jlink.contains("start") ||
                !jlink.contains("end"))
                continue;

            int old_start = jlink["start"].get<int>();
            int old_end   = jlink["end"].get<int>();

            if (id_map.count(old_start) == 0 ||
                id_map.count(old_end) == 0) {

                std::cout << "[Load] Failed link: "
                          << old_start
                          << " -> "
                          << old_end
                          << "\n";

                continue;
            }

            Link lnk;

            lnk.id        = NextID();
            lnk.pin_start = id_map[old_start];
            lnk.pin_end   = id_map[old_end];

            g_pending_links.push_back(lnk);

            loaded_links++;
        }
    }

    std::cout << "[Load] "
              << loaded_nodes
              << " nodes, "
              << loaded_links
              << " links\n";
}