#pragma once
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include "imgui.h"
#include "imnodes.h"
#include "pins/NodePin.h"
#include "nlohmann/json.hpp"

struct NodeOutput {
    std::string output_name;
    PinType type = PinType::String;
    PinValue value;
};

struct NodeBase {
    int id;
    std::string type_id;
    std::string title;
    std::vector<NodePin> inputs;
    std::vector<NodePin> outputs;
    ImVec2 pos;
    ImU32 node_color = IM_COL32(60, 60, 60, 255);
    std::function<NodeOutput(int)> get_input;

    NodeBase(int id, const char* type_id, const char* title):
        id(id), type_id(type_id), title(title), pos(0, 0) {}

    virtual ~NodeBase() = default;

    virtual NodeOutput GetOutput() { return {}; }
    virtual void DrawContent() {}

    virtual bool HasRunButton() { return false; }
    virtual bool IsReadyToRun() { return true; }
    virtual void OnRun() {}
    virtual void OnTick() {}

    virtual nlohmann::json Serialize() const {
        return {
            { "title", title},
            { "type_id",  type_id },
            { "id",    id    },
            { "pos_x", pos.x },
            { "pos_y", pos.y },
            { "input_ids",  [&]{ std::vector<int> v; for (auto& p : inputs)  v.push_back(p.id); return v; }() },
            { "output_ids", [&]{ std::vector<int> v; for (auto& p : outputs) v.push_back(p.id); return v; }() }
        };
    }

    virtual void Deserialize(const nlohmann::json& j) {
    }

    void Draw() {
        ImNodes::PushColorStyle(ImNodesCol_NodeBackground,        node_color);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundHovered, node_color);
        ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected,node_color);
        ImNodes::BeginNode(id);

        ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted(title.c_str());
        ImNodes::EndNodeTitleBar();

        for (auto& pin : inputs) {
            if (pin.node_id != id)
                pin.node_id = id;
            ImNodes::PushColorStyle(ImNodesCol_Pin,        PinColor(pin.type));
            ImNodes::PushColorStyle(ImNodesCol_PinHovered, PinColor(pin.type));
            ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
            ImNodes::BeginInputAttribute(pin.id);
                ImGui::Indent(40);
                ImGui::Text("%.40s", pin.name.c_str());
            ImNodes::EndInputAttribute();
            ImNodes::PopAttributeFlag();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }

        ImGui::PushID(id);
            DrawContent();

            if (HasRunButton()) {
                bool ready = IsReadyToRun();
                if (!ready) ImGui::BeginDisabled();
                if (ImGui::Button("Run")) OnRun();
                if (!ready) ImGui::EndDisabled();
            }
        ImGui::PopID();

        for (auto& pin : outputs) {
            if (pin.node_id != id)
                pin.node_id = id;
            ImNodes::PushColorStyle(ImNodesCol_Pin,        PinColor(pin.type));
            ImNodes::PushColorStyle(ImNodesCol_PinHovered, PinColor(pin.type));
            ImNodes::BeginOutputAttribute(pin.id);
                ImGui::Indent(40);
                ImGui::Text("%.40s", pin.name.c_str());
            ImNodes::EndOutputAttribute();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }

        ImNodes::EndNode();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }
};