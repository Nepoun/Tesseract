#pragma once
#include <unordered_map>
#include "nodes/NodeBase.h"
#include "Link.h"

struct RuntimeNodeRegistry {
    std::unordered_map<int, NodeBase*> nodes;
    std::unordered_map<int, NodePin*>  pins;
    std::unordered_map<int, Link>      links; // Link por valor — sem ponteiro externo

    void RegisterNode(NodeBase* node) {
        nodes[node->id] = node;
        for (auto& pin : node->inputs) {
            pins[pin.id] = &pin;
        }
        for (auto& pin : node->outputs) {
            pins[pin.id] = &pin;
        }
    }

    void RegisterLink(Link link) {   // recebe por valor
        links[link.id] = link;       // cópia fica dentro do map
    }

    void RemoveNode(int id) {
        auto it = nodes.find(id);
        if (it == nodes.end())
            return;

        NodeBase* node = it->second;
        for (auto& pin : node->inputs)  pins.erase(pin.id);
        for (auto& pin : node->outputs) pins.erase(pin.id);

        nodes.erase(it);
    }

    void RemoveLink(int id) {
        links.erase(id);
    }

    // Remove todos os links que tocam qualquer pin de um nó deletado
    void RemoveLinksForNode(NodeBase* node) {
        std::vector<int> to_remove;

        for (const auto& [id, link] : links) {
            for (const auto& pin : node->inputs) {
                if (link.pin_end == pin.id || link.pin_start == pin.id)
                    to_remove.push_back(id);
            }
            for (const auto& pin : node->outputs) {
                if (link.pin_end == pin.id || link.pin_start == pin.id)
                    to_remove.push_back(id);
            }
        }

        for (int id : to_remove)
            links.erase(id);
    }

    NodeBase* GetNode(int id) {
        auto it = nodes.find(id);
        return it != nodes.end() ? it->second : nullptr;
    }

    NodePin* GetPin(int id) {
        auto it = pins.find(id);
        return it != pins.end() ? it->second : nullptr;
    }

    Link* GetLink(int id) {
        auto it = links.find(id);
        return it != links.end() ? &it->second : nullptr;
    }
};

inline RuntimeNodeRegistry& GetRuntimeNodeRegistry() {
    static RuntimeNodeRegistry registry;
    return registry;
}