#pragma once
#include <vector>
#include <functional>
#include <memory>
#include "nodes/NodeBase.h"

struct NodeTypeInfo {
    const char* type_id;
    const char* display_name;
    // Cada factory recebe NextID e cria o nó com os pins que precisar
    std::function<std::unique_ptr<NodeBase>(std::function<int()>)> create;
};

// Registro global — inline garante que existe uma só instância
inline std::vector<NodeTypeInfo>& GetNodeRegistry() {
    static std::vector<NodeTypeInfo> registry;
    return registry;
}