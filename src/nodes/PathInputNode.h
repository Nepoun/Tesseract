#pragma once
#include "NodeBase.h"
#include "util/IdUtil.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <string>
#include <cstring>

struct PathInputNode : public NodeBase {
    std::string path;
    const char* default_path = "C:/Users/purpl/Desktop/NovoEu/BOBimg2pix/test input";
    bool verified    = false;
    bool file_exists = false;

    PathInputNode(int id): NodeBase(id, "path_input_node", "Path Input") {
        node_color = IM_COL32(50, 55, 75, 255);
        outputs.emplace_back(NextID(), id, "Path", PinType::String);
    }

    // Save / Load

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["path"] = path;
        return j;
    }

    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("path")) {
            path = j["path"].get<std::string>();
            verified    = false;
            file_exists = false;
        }
    }

    // Output

    NodeOutput GetOutput() override {
        return { "Path output", PinType::String, path };
    }

    // Helpers

    bool FileExists() const {
        return !path.empty() && std::filesystem::exists(path);
    }

    bool CanVerify() const {
        return !path.empty();
    }

    // Draw

    void DrawContent() override {
        char buffer[512] = {};
        strncpy(buffer, path.c_str(), sizeof(buffer) - 1);

        ImGui::SetNextItemWidth(180);
        if (ImGui::InputText("##path", buffer, sizeof(buffer))) {
            path = buffer;
            NormalizePath(path);
            verified    = false;
            file_exists = false;
        }

        ImGui::Spacing();

        if (!CanVerify()) ImGui::BeginDisabled();
        if (ImGui::SmallButton("Verificar")) {
            verified    = true;
            file_exists = FileExists();
        }
        if (!CanVerify()) ImGui::EndDisabled();

        ImGui::SameLine();

        if (ImGui::SmallButton("Default")) {
            path = default_path;
            NormalizePath(path);
            verified    = false;
            file_exists = false;
        }

        if (!verified)
            return;

        ImGui::Spacing();

        if (file_exists)
            ImGui::TextColored({ 0.3f, 1.0f, 0.3f, 1.0f }, "[OK]");
        else
            ImGui::TextColored({ 1.0f, 0.3f, 0.3f, 1.0f }, "[Nao encontrado]");

        ImGui::Spacing();
        ImGui::TextDisabled("%.60s", path.c_str());
        ImGui::Spacing();

        if (ImGui::SmallButton("Limpar")) {
            path.clear();
            verified    = false;
            file_exists = false;
        }
    }
};