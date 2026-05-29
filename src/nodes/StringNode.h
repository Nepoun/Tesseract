#pragma once

#include "NodeBase.h"
#include "util/IdUtil.h"

struct StringNode : public NodeBase {
    char text_buf[512] = "";

    StringNode(int id): NodeBase(id, "string_node", "Simple String") {
        node_color = Palette(47);
        outputs.emplace_back(NextID(), id, "String", PinType::String);
    }

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["text"] = text_buf;
        return j;
    }
    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("text"))
            strncpy(text_buf,
                    j["text"].get<std::string>().c_str(),
                    sizeof(text_buf));
    }

    NodeOutput GetOutput() override {
        return { "Output string", PinType::String, std::string(text_buf) };
    }

    void DrawContent() override {
        ImGui::SetNextItemWidth(150);
        ImGui::InputText("##value", text_buf, sizeof(text_buf));
    }
};