#pragma once

#include "NodeBase.h"
#include "util/IdUtil.h"
#include "util/Palette.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct PaletteViewerNode : public NodeBase {

    struct PaletteColorData {
        int r;
        int g;
        int b;
        int a;
    };

    std::vector<PaletteColorData> colors;

    bool loaded = false;
    bool failed = false;

    PaletteViewerNode(int id):
        NodeBase(id, "palette_viewer_node", "Palette Viewer")
    {
        node_color = Palette(30);

        inputs.emplace_back(
            NextID(),
            id,
            "Palette Path",
            PinType::String
        );
    }

    bool HasRunButton() override {
        return true;
    }

    bool IsReadyToRun() override {

        auto path_data = get_input(inputs[0].id);

        return std::holds_alternative<std::string>(
            path_data.value
        );
    }

    void OnRun() override {

        loaded = false;
        failed = false;
        colors.clear();

        auto path_data = get_input(inputs[0].id);

        if (!std::holds_alternative<std::string>(
            path_data.value))
        {
            failed = true;
            return;
        }

        std::string path =
            std::get<std::string>(
                path_data.value
            );

        std::ifstream file(path);

        if (!file) {

            std::cout
                << "[Palette Viewer] Failed to open: "
                << path
                << "\n";

            failed = true;
            return;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            if (line[0] == '#')
                continue;

            std::stringstream ss(line);

            PaletteColorData color;

            color.a = 255;

            ss >> color.r
               >> color.g
               >> color.b;

            if (!(ss >> color.a))
                color.a = 255;

            colors.push_back(color);
        }

        loaded = true;

        std::cout
            << "[Palette Viewer] Loaded "
            << colors.size()
            << " colors\n";
    }

    void DrawContent() override {

        if (failed) {

            ImGui::TextColored(
                ImVec4(1.f, 0.3f, 0.3f, 1.f),
                "failed to load"
            );
        }
        if (loaded) {

            ImGui::Text(
                "Colors: %d",
                (int)colors.size()
            );

            ImGui::Spacing();

            if (ImGui::BeginTable(
                "palette_table",
                3,
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_SizingStretchSame |
                ImGuiTableFlags_ScrollY,
                ImVec2(260, 250)
            ))
            
            {
                ImGui::TableSetupColumn("ID",    ImGuiTableColumnFlags_WidthFixed,   40.f);
                ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed,   60.f);
                ImGui::TableSetupColumn("RGBA",  ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (size_t i = 0; i < colors.size(); i++)
                {
                    auto& c = colors[i];

                    ImGui::TableNextRow();

                    ImGui::PushID((int)i);

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", (int)i);

                    ImGui::TableSetColumnIndex(1);

                    ImGui::ColorButton(
                        "##color",
                        ImVec4(
                            c.r / 255.f,
                            c.g / 255.f,
                            c.b / 255.f,
                            c.a / 255.f
                        ),
                        ImGuiColorEditFlags_NoTooltip,
                        ImVec2(24, 24)
                    );

                    ImGui::TableSetColumnIndex(2);

                    ImGui::Text(
                        "%d %d %d %d",
                        c.r,
                        c.g,
                        c.b,
                        c.a
                    );

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
    }
};