#pragma once

#include "NodeBase.h"
#include "util/IdUtil.h"

struct StringNode : public NodeBase {
    char text_buf[512] = "";

    StringNode(int id): NodeBase(id, "Simple String") {
        node_color = IM_COL32(50, 75, 55, 255); 
        outputs.emplace_back(NextID(), id, "String", PinType::String);
    }

    NodeOutput GetOutput() override {
        return { "Output string", PinType::String, std::string(text_buf) };
    }

    void DrawContent() override {
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##value", text_buf, sizeof(text_buf));
    }
};