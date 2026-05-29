#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "imnodes.h"

#include "pins/NodePin.h"
#include "pins/Link.h"

#include "nodes/NodeBase.h"
#include "nodes/PresetNode.h"
#include "nodes/ConversorNode.h"
#include "nodes/ViewDataNode.h"
#include "nodes/StringNode.h"
#include "nodes/PathInputNode.h"

#include "registry/NodeRegistry.h"
#include "registry/RuntimeNodeRegistry.h"

#include "util/IdUtil.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>

// TODO
// 1) Cor por tipo de node
// 2) Cor por tipo de input e output
// 3) Deixar mais generalizado e modular input e output (por exemplo matar aquela palhaçada de Run() do conversor)
// 4) Arrumar uma barra de carregamento melhor
// 5) Fazer o conversor apagar imagens
// 6) Node de preview (Mesma coisa que o de preset) que mostre algumas das imagens com conversão em uma pasta temp e apague elas depois de mostrar 
// 7) Node de conversor poder refazer
// 8) Node de edição de preset

// BACKBURNER
// 1) Parar de usar o slk 
// 2) Tipo de input "Textura", "UV"
// 3) Multiply, add e blend

NodeOutput GetInputData(int input_pin_id)
{
    auto& registry = GetRuntimeNodeRegistry();

    for (const auto& [id, link] : registry.links){
        if (link.pin_end != input_pin_id)
            continue;
        
        NodePin* output_pin = registry.GetPin(link.pin_start);

        if (!output_pin)
            return {};
        
        NodeBase* owner_node = registry.GetNode(output_pin->node_id);

        if(!owner_node)
            return {};
        
        return owner_node->GetOutput();
    }

    return {};
}

int main(int argc, char* argv[])
{
    // =========================
    // SDL
    // =========================

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Tesseract Conversor",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    // Cria o contexto OpenGL vinculado á janela
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    // Basicamente vsync 
    SDL_GL_SetSwapInterval(1);


    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImNodes::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::vector<std::unique_ptr<NodeBase>> nodes;
    GetNodeRegistry().push_back({
        "Preset Node",
        [](std::function<int()> next) {
            int id = next();
            return std::make_unique<PresetNode>(id);
        }
    });
    
    GetNodeRegistry().push_back({
        "Conversor Node",
        [](std::function<int()> next) {
            int id         = next();
            return std::make_unique<ConversorNode>(id);
        }
    });

    GetNodeRegistry().push_back({
        "View Data Node",
        [](std::function<int()> next) {
            return std::make_unique<ViewDataNode>(next());
        }
    });

    GetNodeRegistry().push_back({
        "String Creator Node",
        [](std::function<int()> next) {
            return std::make_unique<StringNode>(next());
        }
    });

    GetNodeRegistry().push_back({
        "Path Formatter Node",
        [](std::function<int()> next) {
            return std::make_unique<PathInputNode>(next());
        }
    });

    bool running = true;

    while (running) {
        // Processa eventos do sistema operacional (Teclado, mouse, redimensionar janela...)
        SDL_Event event;

        //Retorna true enquanto tem eventos na fila
        while (SDL_PollEvent(&event)) {
            // Passa o evento para o ImGui processar (cliques, texto digitado)
            ImGui_ImplSDL2_ProcessEvent(&event);
 
            // Verifica se o usuário fechou a janela
            if (event.type == SDL_QUIT)
                running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("BOB Visual Conversor", nullptr,
            ImGuiWindowFlags_NoTitleBar     |
            ImGuiWindowFlags_NoResize       |
            ImGuiWindowFlags_NoMove         |
            ImGuiWindowFlags_NoScrollbar    |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );
        ImNodes::BeginNodeEditor();
        
            static bool first_frame = true;
            if (first_frame) {
                for (auto& node : nodes){
                    ImNodes::SetNodeGridSpacePos(node->id, node->pos);
                }
                first_frame = false;
            }


            for (auto& node : nodes) {
                node->get_input = GetInputData;
                node->Draw();
                node->pos = ImNodes::GetNodeGridSpacePos(node->id);
                node->OnTick();
            }

            for (const auto& [id, link] : GetRuntimeNodeRegistry().links) {
                // Acha o pin de saída para pegar o tipo
                NodePin* pin = GetRuntimeNodeRegistry().GetPin(link.pin_start);
                if (pin) {
                    ImNodes::PushColorStyle(ImNodesCol_Link,        PinColor(pin->type));
                    ImNodes::PushColorStyle(ImNodesCol_LinkHovered, PinColor(pin->type));
                    ImNodes::PushColorStyle(ImNodesCol_LinkSelected,PinColor(pin->type));
                }
                ImNodes::Link(link.id, link.pin_start, link.pin_end);
                if (pin) {
                    ImNodes::PopColorStyle();
                    ImNodes::PopColorStyle();
                    ImNodes::PopColorStyle();
                }
            }

        ImNodes::EndNodeEditor();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("context_menu");

        if (ImGui::BeginPopupContextWindow("context_menu",
            ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            ImVec2 spawn_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

            for (auto& type : GetNodeRegistry()) {
                if (ImGui::MenuItem(type.display_name)) {
                    auto node = type.create(NextID);
                    node->pos = spawn_pos;
                    ImNodes::SetNodeScreenSpacePos(node->id, spawn_pos);
                    nodes.push_back(std::move(node));
                    GetRuntimeNodeRegistry().RegisterNode(nodes.back().get());
                }
            }

            ImGui::EndPopup();
        }

        {
            int start_pin, end_pin;

            if (ImNodes::IsLinkCreated(&start_pin, &end_pin)){
                Link new_link;
                new_link.id = NextID();
                new_link.pin_start = start_pin;
                new_link.pin_end = end_pin;

                GetRuntimeNodeRegistry().RegisterLink(new_link);
            }
        }

        {
            int destroyed_id;
            if (ImNodes::IsLinkDestroyed(&destroyed_id))
                GetRuntimeNodeRegistry().RemoveLink(destroyed_id);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            int count = ImNodes::NumSelectedNodes();
            if (count > 0){
                std::vector<int> selected_ids(count);
                ImNodes::GetSelectedNodes(selected_ids.data());
 
                // Remove do registry (links órfãos incluídos) antes de destruir os nós
                for (int id : selected_ids) {
                    NodeBase* node = GetRuntimeNodeRegistry().GetNode(id);
                    if (node) GetRuntimeNodeRegistry().RemoveLinksForNode(node);
                    GetRuntimeNodeRegistry().RemoveNode(id);
                }
 
                // Remove do vetor de nós
                nodes.erase(
                    std::remove_if(nodes.begin(), nodes.end(),
                        [&selected_ids](const std::unique_ptr<NodeBase>& node) {
                            return std::find(
                                selected_ids.begin(),
                                selected_ids.end(),
                                node->id
                            ) != selected_ids.end();
                        }
                    ),
                    nodes.end()
                );
            }
        }


        ImGui::End();

        ImGui::Render();

        glViewport(0, 0,
            (int)ImGui::GetIO().DisplaySize.x,
            (int)ImGui::GetIO().DisplaySize.y
        );

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // cor fundo
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }
    // Destruir tudo que tu cria em ordem inversa
    // Nodes vector é destruido ao sair de escopo automaticamente
    // O motivo p usar unique_ptr é para não precisar deletar cada nó manualmente

    ImNodes::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
 
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}