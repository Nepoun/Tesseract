#pragma once

#include "NodeBase.h"
#include "util/IdUtil.h"
#include "types/Preset.h"

struct ViewDataNode : public NodeBase {
    NodeOutput cached_input;
    bool has_input = false;

    ViewDataNode(int id): NodeBase(id, "dataview_node", "View Data") {
        node_color = IM_COL32(55, 70, 75, 255);
        inputs.emplace_back(NextID(), id, "Value", PinType::String);
    }

    void SetInput(NodeOutput data) {
        cached_input = data;
        has_input    = true;
    }

    void OnTick() override {
        SetInput(get_input(inputs[0].id));
    }

    void DrawContent() override {
        ImGui::SetNextItemWidth(160);
        if (!has_input) {
            ImGui::TextDisabled("Sem conexão");
            return;
        }
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>)
                ImGui::Text("Int: %d", val);
            else if constexpr (std::is_same_v<T, float>)
                ImGui::Text("Float: %.4f", val);
            else if constexpr (std::is_same_v<T, bool>)
                ImGui::Text("Bool: %s", val ? "true" : "false");
            else if constexpr (std::is_same_v<T, std::string>) {
                ImGui::Text("String:");
                ImGui::TextDisabled("%.60s", val.c_str());
            }
            else if constexpr (std::is_same_v<T, Preset>) {
                ImGui::Text("Preset:");
                ImGui::Text("  %.40s", val.name);
                ImGui::TextDisabled("  %.60s", val.path);
            }
        }, cached_input.value);
    }
};