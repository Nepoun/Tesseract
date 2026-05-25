#pragma once
#include "NodeBase.h"
#include <filesystem>
#include <future>
#include <atomic>
#include <string>

#include "util/IdUtil.h"

struct ConversorNode : public NodeBase {

    // Caminho da pasta de saída (esse nó define onde salva)
    char output_folder[512] = "output/";

    // Estado da conversão
    enum class Status { Idle, Running, Done, Error };
    Status status = Status::Idle;

    std::atomic<int> converted = 0;  // quantas imagens já processaram
    int total = 0;                   // total de imagens na pasta

    // Guarda o future para checar quando terminar sem bloquear a UI
    std::future<void> task;

    ConversorNode(int id): NodeBase(id, "Conversor"){

        inputs.emplace_back(
            NextID(),
            id,
            "Preset",
            PinType::Preset
        );

        inputs.emplace_back(
            NextID(),
            id,
            "Folder Path",
            PinType::String
        );

        inputs.emplace_back(
            NextID(),
            id,
            "EXE Prefix",
            PinType::String
        );

        outputs.emplace_back(
            NextID(),
            id,
            "Image Output Path",
            PinType::String
        );

    }
    void DrawContent() override {

        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Saida", output_folder, sizeof(output_folder));

        ImGui::Spacing();

        // Botão de converter — só ativo se não estiver rodando
        bool can_run = (status != Status::Running);
        if (!can_run) ImGui::BeginDisabled();

        if (ImGui::Button("Converter")) {
            // os dados vêm pelos pins — ConversorNode::Run() recebe de fora
            // (veja como chamar no main.cpp abaixo)
            status = Status::Running;
        }

        if (!can_run) ImGui::EndDisabled();

        ImGui::SameLine();

        // Status visual
        switch (status) {
            case Status::Idle:    ImGui::TextDisabled("aguardando"); break;
            case Status::Running: ImGui::Text("%d / %d", converted.load(), total); break;
            case Status::Done:    ImGui::TextColored({0,1,0,1}, "concluido!"); break;
            case Status::Error:   ImGui::TextColored({1,0,0,1}, "erro"); break;
        }
    }

    void Run(const char* preset_path, const char* folder_in, const char* exe_path)
    {
        if (task.valid())
            return;

        if (!preset_path || !folder_in) {
            status = Status::Error;
            return;
        }

        namespace fs = std::filesystem;

        total = 0;
        converted = 0;

        try {

            for (const auto& entry : fs::directory_iterator(folder_in)) {

                auto ext = entry.path().extension().string();

                if (
                    ext == ".png" ||
                    ext == ".jpg" ||
                    ext == ".bmp" ||
                    ext == ".tga"
                ) {
                    total++;
                }
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
        std::string exe = exe_path;
        std::string out_dir = output_folder;

        task = std::async(std::launch::async,
            [this, preset, in_dir, out_dir, exe]()
        {
            namespace fs = std::filesystem;

            try {

                fs::create_directories(out_dir);

                for (const auto& entry : fs::directory_iterator(in_dir)) {

                    auto ext = entry.path().extension().string();

                    if (
                        ext != ".png" &&
                        ext != ".jpg" &&
                        ext != ".bmp" &&
                        ext != ".tga"
                    ) {
                        continue;
                    }

                    std::string in_file =
                        entry.path().string();

                    std::string out_file =
                        out_dir + "/" +
                        entry.path().stem().string() +
                        ".png";


                    std::string cmd =
                        exe +
                        " --in \"" + in_file + "\""
                        " --out \"" + out_file + "\""
                        " --preset \"" + preset + "\"";

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
        task = {};
    }

    
    void FuncaoQueOJoaoTaSegurando(const char* preset_path, const char* folder_in) {
        if (!preset_path || !folder_in) {
            status = Status::Error;
            return;
        }

        namespace fs = std::filesystem;
        total = 0;
        converted = 0;

        for (const auto& entry : fs::directory_iterator(folder_in)) {
            auto ext = entry.path().extension().string();
            if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga")
                total++;
        }

        if (total == 0) { status = Status::Error; return; }

        std::string preset  = preset_path;
        std::string in_dir  = folder_in;
        std::string out_dir = output_folder;

        std::thread([this, preset, in_dir, out_dir]() {
            namespace fs = std::filesystem;
            fs::create_directories(out_dir);

            for (const auto& entry : fs::directory_iterator(in_dir)) {
                auto ext = entry.path().extension().string();
                if (ext != ".png" && ext != ".jpg" && ext != ".bmp" && ext != ".tga")
                    continue;

                std::string in_file  = entry.path().string();
                std::string out_file = out_dir + "/" + entry.path().stem().string() + ".png";

                std::string cmd = "slk/SLK_img2pix_cmd.exe"
                    " --in \""  + in_file  + "\""
                    " --out \"" + out_file + "\""
                    " --preset \"" + preset + "\"";

                std::system(cmd.c_str());
                converted++;
            }

            status = Status::Done;
        }).detach(); 
    }
};