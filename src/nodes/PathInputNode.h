#pragma once

#include "NodeBase.h"
#include "util/IdUtil.h"

#include <filesystem>
#include <string>

struct PathInputNode : public NodeBase {
    std::string path;

    const char* default_path = "C:/Users/purpl/Desktop/NovoEu/BOBimg2pix/test input";

    bool verified = false;
    bool file_exists = false;

    PathInputNode(int id)
        : NodeBase(id, "Path Input")
    {
        outputs.emplace_back(
            NextID(),
            id,
            "Path",
            PinType::String
        );
    }

    NodeOutput GetOutput() override {
        return {
            "Path output",
            PinType::String,
            path
        };
    }

    bool FileExists() const {
        return !path.empty() &&
               std::filesystem::exists(path);
    }

    bool CanVerify() const {
        return !path.empty();
    }

    void DrawContent() override {
        // ── Input ──────────────────────────────────────
        char buffer[512];
        memset(buffer, 0, sizeof(buffer));

        strncpy(buffer, path.c_str(), sizeof(buffer) - 1);

        ImGui::SetNextItemWidth(180);

        if (ImGui::InputText("Caminho", buffer, sizeof(buffer)))
        {
            path = buffer;

            NormalizePath(path);

            // invalida verificação antiga
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

        if (ImGui::SmallButton("Default"))
        {
            path = default_path;
            NormalizePath(path);

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

            ImGui::TextDisabled("%.60s", path.c_str());

            ImGui::Spacing();

            if (ImGui::SmallButton("Limpar"))
            {
                path.clear();

                verified = false;
                file_exists = false;
            }
        }
    }
};