#pragma once
#include "NodeBase.h"
#include <filesystem>
#include <future>
#include <atomic>
#include <string>
#include "util/IdUtil.h"
#include "util/Palette.h"

struct ConversorNode : public NodeBase {

    char output_folder[512] = "output/";
    enum class Status { Idle, Running, Done, Error };
    Status status = Status::Idle;
    
    std::atomic<int> converted = 0;
    int total = 0;
    
    std::future<void> task;

    ConversorNode(int id): NodeBase(id, "conversor_node", "Conversor") {
        node_color = Palette(3);
        inputs.emplace_back(NextID(), id, "Preset",      PinType::Preset);
        inputs.emplace_back(NextID(), id, "Folder Path", PinType::String);
        inputs.emplace_back(NextID(), id, "EXE Prefix",  PinType::String);
        outputs.emplace_back(NextID(), id, "Image Output Path", PinType::String);
    }

    // -------------------------
    // Contrato com NodeBase
    // -------------------------

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["output_folder"] = output_folder;
        return j;
    }
    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("output_folder"))
            strncpy(output_folder,
                    j["output_folder"].get<std::string>().c_str(), 
                    sizeof(output_folder));
    }
        
    bool HasRunButton() override {
        return true;
    }

    bool IsReadyToRun() override {
        if (status == Status::Running)
            return false;

        auto preset = get_input(inputs[0].id);
        auto folder = get_input(inputs[1].id);

        return std::holds_alternative<Preset>(preset.value) &&
               std::holds_alternative<std::string>(folder.value);
    }

    void OnRun() override {
        auto preset_data = get_input(inputs[0].id);
        auto folder_data = get_input(inputs[1].id);
        auto exe_data    = get_input(inputs[2].id);

        // Guardamos em strings locais antes de pegar o c_str()
        // para evitar ponteiros para memória destruída
        std::string preset_str, folder_str, exe_str;

        const char* preset_path = nullptr;
        const char* folder_path = nullptr;
        const char* exe_path    = nullptr;

        if (std::holds_alternative<Preset>(preset_data.value)) {
            preset_str  = std::get<Preset>(preset_data.value).path; // copia pra preset_str
            preset_path = preset_str.c_str();                       // ponteiro seguro
        }

        if (std::holds_alternative<std::string>(folder_data.value)) {
            folder_str  = std::get<std::string>(folder_data.value);
            folder_path = folder_str.c_str();
        }

        if (std::holds_alternative<std::string>(exe_data.value)) {
            exe_str  = std::get<std::string>(exe_data.value);
            exe_path = exe_str.c_str();
        }

        Run(preset_path, folder_path, exe_path);
    }

    void DrawContent() override {
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Saida", output_folder, sizeof(output_folder));
        ImGui::Spacing();

        switch (status) {
            case Status::Idle:    ImGui::TextDisabled("aguardando");               break;
            case Status::Running: ImGui::Text("%d / %d", converted.load(), total); break;
            case Status::Done:    ImGui::TextColored({0,1,0,1}, "concluido!");      break;
            case Status::Error:   ImGui::TextColored({1,0,0,1}, "erro");            break;
        }
    }

    // -------------------------
    // Lógica interna
    // -------------------------

    void Run(const char* preset_path, const char* folder_in, const char* exe_path) {
        if (task.valid()){
            if (status == Status::Running){
                return;
            }
        }

        if (!preset_path || !folder_in) {
            status = Status::Error;
            return;
        }

        namespace fs = std::filesystem;

        total     = 0;
        converted = 0;

        try {
            for (const auto& entry : fs::directory_iterator(folder_in)) {
                auto ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga")
                    total++;
            }
        } catch (...) {
            status = Status::Error;
            return;
        }

        if (total == 0) {
            status = Status::Error;
            return;
        }

        status = Status::Running;

        std::string preset  = preset_path;
        std::string in_dir  = folder_in;
        std::string exe     = exe_path ? exe_path : "";
        std::string out_dir = output_folder;

        task = std::async(std::launch::async,
            [this, preset, in_dir, out_dir, exe]()
        {
            namespace fs = std::filesystem;
            try {
                fs::create_directories(out_dir);

                for (const auto& entry : fs::directory_iterator(in_dir)) {
                    auto ext = entry.path().extension().string();
                    if (ext != ".png" && ext != ".jpg" && ext != ".bmp" && ext != ".tga")
                        continue;

                    std::string in_file  = entry.path().string();
                    std::string out_file = out_dir + "/" + entry.path().stem().string() + ".png";

                    std::string cmd =
                        exe +
                        " --in \""     + in_file  + "\""
                        " --out \""    + out_file + "\""
                        " --preset \"" + preset   + "\"";

                    int result = std::system(cmd.c_str());

                    if (result != 0) {
                        status = Status::Error;
                        return;
                    }

                    converted++;
                }

                status = Status::Done;

            } catch (...) {
                status = Status::Error;
            }
        });
    }
};