#pragma once
#include "NodeBase.h"
#include "types/Preset.h"
#include "util/IdUtil.h"
#include "util/Palette.h"

#include <fstream>
#include <filesystem>

struct PresetEditorNode : public NodeBase {

    Preset     preset_ref;
    PresetData data;

    bool saved = false;
    bool error = false;

    PresetEditorNode(int id)
        : NodeBase(id, "preset_editor_node", "Preset Editor")
    {
        node_color = Palette(12);
        inputs.emplace_back(NextID(), id, "Preset",      PinType::Preset);
        inputs.emplace_back(NextID(), id, "Output Path", PinType::String);
        outputs.emplace_back(NextID(), id, "Preset",     PinType::Preset);
    }

    // ------------------------------------------------------------------ I/O

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["output_path"] = preset_ref.path;
        j.merge_patch(data.ToJson());
        return j;
    }

    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        preset_ref.path = j.value("output_path", std::string{});
        data.FromJson(j);
    }

    NodeOutput GetOutput() override {
        return {"Preset", PinType::Preset, preset_ref};
    }

    // --------------------------------------------------------- Run / Ready

    bool HasRunButton() override { return true; }

    bool IsReadyToRun() override {
        return
            std::holds_alternative<Preset>(get_input(inputs[0].id).value) &&
            std::holds_alternative<std::string>(get_input(inputs[1].id).value);
    }

    void OnRun() override {
        saved = error = false;

        auto preset_in = get_input(inputs[0].id);
        auto path_in   = get_input(inputs[1].id);

        if (!std::holds_alternative<Preset>(preset_in.value) ||
            !std::holds_alternative<std::string>(path_in.value))
        {
            error = true;
            return;
        }

        preset_ref = std::get<Preset>(preset_in.value);

        const std::string& folder = std::get<std::string>(path_in.value);
        if (folder.empty()) { error = true; return; }

        std::filesystem::path out_path =
            std::filesystem::path(folder) / "temp_output.json";

        try {
            std::filesystem::create_directories(out_path.parent_path());

            nlohmann::json j = data.ToJson();
            MergeExistingPaletteKeys(preset_ref.path, j);

            std::ofstream out(out_path);
            out << j.dump(4);

            preset_ref.path = out_path.string();
            saved = true;

        } catch (...) {
            error = true;
        }
    }

    // ------------------------------------------------------- DrawContent

    void DrawContent() override {
        ImGui::Dummy(ImVec2(0, 4));

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 6);
        if (ImGui::Button("Carregar do Preset"))
            LoadValuesFromPreset();

        ImGui::BeginChild("preset_editor_child", ImVec2(320, 360), false);
        ImGui::PushItemWidth(180);

        if (ImGui::BeginTabBar("preset_tabs")) {
            DrawTransformTab();
            DrawColorTab();
            DrawDitherTab();
            ImGui::EndTabBar();
        }

        ImGui::PopItemWidth();
        ImGui::Spacing();

        if (saved) ImGui::TextColored({0,1,0,1}, "Preset salvo com sucesso");
        if (error) ImGui::TextColored({1,0,0,1}, "Erro ao salvar preset");

        ImGui::EndChild();
    }

private:

    // --------------------------------------------------- Tabs

    void DrawTransformTab() {
        if (!ImGui::BeginTabItem("Transform")) return;

        ImGui::SeparatorText("Sampling");
        ImGui::DragInt("Sample Mode", &data.sample_mode, 1, 0, 16);

        ImGui::SeparatorText("Scale");
        ImGui::Checkbox("Scale Relative", &data.scale_relative);
        if (data.scale_relative) {
            ImGui::DragInt("Scale X", &data.size_relative_x, 1, 1, 64);
            ImGui::DragInt("Scale Y", &data.size_relative_y, 1, 1, 64);
        } else {
            ImGui::DragInt("Width",  &data.size_absolute_x, 1, 1, 4096);
            ImGui::DragInt("Height", &data.size_absolute_y, 1, 1, 4096);
        }

        ImGui::SeparatorText("Offsets");
        ImGui::DragFloat("X Offset", &data.x_offset, 0.01f);
        ImGui::DragFloat("Y Offset", &data.y_offset, 0.01f);

        ImGui::SeparatorText("Filter");
        ImGui::DragFloat("Blur",  &data.blur_amount,  0.001f, 0.0f, 10.0f);
        ImGui::DragFloat("Sharp", &data.sharp_amount, 0.001f, 0.0f, 10.0f);

        ImGui::EndTabItem();
    }

    void DrawColorTab() {
        if (!ImGui::BeginTabItem("Color")) return;

        ImGui::SeparatorText("Adjustments");
        ImGui::DragFloat("Brightness", &data.brightness, 0.001f, -10.0f, 10.0f);
        ImGui::DragFloat("Contrast",   &data.contrast,   0.001f, -10.0f, 10.0f);
        ImGui::DragFloat("Saturation", &data.saturation, 0.001f, -10.0f, 10.0f);
        ImGui::DragFloat("Hue",        &data.hue,        0.1f,  -360.0f, 360.0f);
        ImGui::DragFloat("Gamma",      &data.gamma,      0.001f,  0.01f,   5.0f);

        ImGui::SeparatorText("Tint");
        ImGui::DragInt("Red",   &data.tint_red,   1, 0, 255);
        ImGui::DragInt("Green", &data.tint_green, 1, 0, 255);
        ImGui::DragInt("Blue",  &data.tint_blue,  1, 0, 255);

        ImGui::EndTabItem();
    }

    void DrawDitherTab() {
        if (!ImGui::BeginTabItem("Dither")) return;

        ImGui::Checkbox("KMeans++", &data.kmeanspp);
        ImGui::Separator();
        ImGui::DragInt  ("Alpha Threshold", &data.alpha_threshold, 1,      0, 255);
        ImGui::DragFloat("Dither Amount",   &data.dither_amount,   0.001f, 0.0f, 10.0f);
        ImGui::DragInt  ("Target Colors",   &data.target_colors,   1,      1, 256);
        ImGui::DragInt  ("Dither Mode",     &data.dither_mode,     1,      0,  32);
        ImGui::DragInt  ("Color Distance",  &data.color_dist,      1,      0,  32);

        ImGui::EndTabItem();
    }

    // ------------------------------------------------- Helpers

    void LoadValuesFromPreset() {
        auto input = get_input(inputs[0].id);
        if (!std::holds_alternative<Preset>(input.value)) return;
        preset_ref = std::get<Preset>(input.value);
        data = PresetData::LoadFromFile(preset_ref.path);
    }

    static void MergeExistingPaletteKeys(const std::string& path, nlohmann::json& target) {
        std::ifstream f(path);
        if (!f) return;
        try {
            nlohmann::json old;
            f >> old;
            for (const char* key : {"dither_palette", "dither_palette_colors"})
                if (old.contains(key)) target[key] = old[key];
        } catch (...) {}
    }
};