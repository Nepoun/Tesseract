#pragma once

#include <cstdlib>

#include <string>
#include <vector>
#include "imgui.h"
#include "imnodes.h"


#include "NodePin.h"
struct NodeOutput {
    std::string output_name;
    PinType type = PinType::String;
    PinValue value;
};

struct NodeBase {
    int id;
    std::string title;
    std::vector<NodePin> inputs;
    std::vector<NodePin> outputs;
    ImVec2 pos;

    NodeBase(int id, const char* title):
        id(id), title(title), pos(0, 0) {}

    virtual ~NodeBase() = default;
    virtual NodeOutput GetOutput() { return {}; }


    virtual void DrawContent() = 0;

    void Draw() {
        // Needed always
        ImNodes::BeginNode(id);

        // Title bar preparation
        ImNodes::BeginNodeTitleBar();
            ImGui::TextUnformatted(title.c_str());
        ImNodes::EndNodeTitleBar();

        for (auto& pin : inputs){

            if (pin.node_id != id){
                pin.node_id = id;
            }

            ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
            ImNodes::BeginInputAttribute(pin.id);
                
                ImGui::Indent(40);
                ImGui::Text("%.40s", pin.name.c_str());

            ImNodes::EndInputAttribute();
            ImNodes::PopAttributeFlag();

        }

        ImGui::PushID(id);
            DrawContent();
        ImGui::PopID();

        for (auto& pin : outputs){

            if (pin.node_id != id){
                pin.node_id = id;
            }

            ImNodes::BeginOutputAttribute(pin.id);
                ImGui::Indent(40);
                ImGui::Text("%.40s", pin.name.c_str());
            ImNodes::EndOutputAttribute();
        }
        ImNodes::EndNode();

    }
};