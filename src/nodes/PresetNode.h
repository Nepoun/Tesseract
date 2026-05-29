#pragma once
 
#include "NodeBase.h"
#include "util/IdUtil.h"
#include "types/Preset.h"
#include <cstring>
#include <filesystem>

struct PresetNode : public NodeBase {
    Preset preset;

    bool verified = false;
    bool file_exists = false;

    const char* default_name = "Default";
    const char* default_path = "C:/Users/purpl/Desktop/NovoEu/BOBimg2pix/build/presettarget.json";

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["preset"] = SerializePreset(preset);
        return j;
    }

    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("preset"))
            preset = DeserializePreset(j["preset"]);
    }

    PresetNode(int id):
        NodeBase(id, "preset_node", "Preset") {
            node_color = IM_COL32(50, 55, 75, 255);
            outputs.emplace_back( NextID(), id, "Preset", PinType::Preset);
        }

    NodeOutput GetOutput() override {
        return { "Preset path output", PinType::Preset, preset };
    }

    bool FileExists() const {
        return preset.path[0] != '\0' &&
               std::filesystem::exists(preset.path);
    }

    bool CanVerify() const {
        return preset.name[0] != '\0' &&
               preset.path[0] != '\0';
    }

    void DrawContent() override {
        // ── Inputs ─────────────────────────────────────
        ImGui::SetNextItemWidth(180);
        ImGui::InputText("Nome", preset.name, sizeof(preset.name));

        ImGui::SetNextItemWidth(180);
        if (ImGui::InputText("Caminho", preset.path, sizeof(preset.path)))
        {
            // Normaliza separadores
            for (char* c = preset.path; *c; ++c)
                if (*c == '/')
                    *c = '\\';

            // Mudou caminho = invalida verificação antiga
            verified = false;
        }

        ImGui::Spacing();

        // ── Botão verificar ────────────────────────────
        if (!CanVerify()) {
            ImGui::BeginDisabled();
        }

        if (ImGui::SmallButton("Verificar"))
        {
            verified = true;
            file_exists = FileExists();
        }

        if (!CanVerify()) {
            ImGui::EndDisabled();
        }

        if (ImGui::SmallButton("Criar Default"))
        {
            std::strncpy(preset.name, default_name, sizeof(preset.name));
            preset.name[sizeof(preset.name) - 1] = '\0';

            std::strncpy(preset.path, default_path, sizeof(preset.path));
            preset.path[sizeof(preset.path) - 1] = '\0';

            // normaliza barras
            for (char* c = preset.path; *c; ++c)
                if (*c == '/')
                    *c = '\\';

            verified = false;
            file_exists = false;
        }


        // ── Resultado ──────────────────────────────────
        if (verified)
        {
            ImGui::Spacing();

            if (file_exists) {
                ImGui::TextColored(
                    ImVec4(0.3f, 1.0f, 0.3f, 1.0f),
                    "[OK]"
                );
            }
            else {
                ImGui::TextColored(
                    ImVec4(1.0f, 0.3f, 0.3f, 1.0f),
                    "[Arquivo nao encontrado]"
                );
            }

            ImGui::Spacing();

            if (ImGui::SmallButton("Limpar"))
            {
                preset.name[0] = '\0';
                preset.path[0] = '\0';

                verified = false;
                file_exists = false;
            }
        }
    }
};