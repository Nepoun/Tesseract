#pragma once
#include "NodeBase.h"
#include "util/IdUtil.h"
#include "types/Preset.h"
#include <filesystem>

struct PresetNode : public NodeBase {

    Preset preset;
    bool verified    = false;
    bool file_exists = false;

    // Buffers separados só para o ImGui::InputText
    char buf_name[128] = "";
    char buf_path[512] = "";

    static constexpr const char* DEFAULT_NAME = "Default";
    static constexpr const char* DEFAULT_PATH =
        "C:/Users/purpl/Desktop/NovoEu/BOBimg2pix/build/presettarget.json";

    PresetNode(int id)
        : NodeBase(id, "preset_node", "Preset")
    {
        node_color = Palette(47);
        outputs.emplace_back(NextID(), id, "Preset", PinType::Preset);
    }

    // ------------------------------------------------------------------ I/O

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["preset"] = SerializePreset(preset);
        return j;
    }

    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("preset")) {
            preset = DeserializePreset(j["preset"]);
            SyncBuffersFromPreset();
        }
    }

    NodeOutput GetOutput() override {
        return {"Preset path output", PinType::Preset, preset};
    }

    // ------------------------------------------------------------------ Draw

    void DrawContent() override {

        ImGui::SetNextItemWidth(180);
        if (ImGui::InputText("Nome", buf_name, sizeof(buf_name))) {
            preset.name = buf_name;
            verified = false;
        }

        ImGui::SetNextItemWidth(180);
        if (ImGui::InputText("Caminho", buf_path, sizeof(buf_path))) {
            for (char* c = buf_path; *c; ++c)
                if (*c == '/') *c = '\\';
            preset.path = buf_path;
            verified = false;
        }

        ImGui::Spacing();

        // Botão Verificar
        const bool can_verify = !preset.name.empty() && !preset.path.empty();
        if (!can_verify) ImGui::BeginDisabled();
        if (ImGui::SmallButton("Verificar")) {
            verified    = true;
            file_exists = std::filesystem::exists(preset.path);
        }
        if (!can_verify) ImGui::EndDisabled();

        // Botão Default
        if (ImGui::SmallButton("Criar Default")) {
            preset.name = DEFAULT_NAME;
            preset.path = DEFAULT_PATH;
            for (char& c : preset.path)
                if (c == '/') c = '\\';
            SyncBuffersFromPreset();
            verified    = false;
            file_exists = false;
        }

        // Resultado da verificação
        if (verified) {
            ImGui::Spacing();
            if (file_exists)
                ImGui::TextColored({0.3f, 1.0f, 0.3f, 1.0f}, "[OK]");
            else
                ImGui::TextColored({1.0f, 0.3f, 0.3f, 1.0f}, "[Arquivo nao encontrado]");

            ImGui::Spacing();
            if (ImGui::SmallButton("Limpar")) {
                preset      = {};
                buf_name[0] = buf_path[0] = '\0';
                verified    = false;
                file_exists = false;
            }
        }
    }

private:

    void SyncBuffersFromPreset() {
        strncpy(buf_name, preset.name.c_str(), sizeof(buf_name) - 1);
        buf_name[sizeof(buf_name) - 1] = '\0';
        strncpy(buf_path, preset.path.c_str(), sizeof(buf_path) - 1);
        buf_path[sizeof(buf_path) - 1] = '\0';
    }
};