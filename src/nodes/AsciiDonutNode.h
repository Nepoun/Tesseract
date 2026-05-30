#pragma once
#include "NodeBase.h"
#include "util/IdUtil.h"
#include "util/Palette.h"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstring>

struct DonutNode : public NodeBase {

    static constexpr int W   = 40;
    static constexpr int H   = 20;
    static constexpr int BUF = W * H;

    float    A = 0.f, B = 0.f;
    char     screen[BUF + 1]{};
    float    zbuf[BUF]{};
    uint64_t last_ticks = 0;

    int speed     = 3;
    int size      = 5;
    int thickness = 2;
    int density   = 6;

    DonutNode(int id) : NodeBase(id, "ascii_donut_node", "Ascii Donut") {
        node_color = Palette(21);
    }

    nlohmann::json Serialize() const override {
        auto j = NodeBase::Serialize();
        j["speed"]     = speed;
        j["size"]      = size;
        j["thickness"] = thickness;
        j["density"]   = density;
        return j;
    }

    void Deserialize(const nlohmann::json& j) override {
        NodeBase::Deserialize(j);
        if (j.contains("speed"))     speed     = j["speed"];
        if (j.contains("size"))      size      = j["size"];
        if (j.contains("thickness")) thickness = j["thickness"];
        if (j.contains("density"))   density   = j["density"];
    }

    NodeOutput GetOutput() override { return {}; }

    void OnTick() override {
        uint64_t now = SDL_GetTicks64();
        if (last_ticks == 0) last_ticks = now;
        float dt   = (now - last_ticks) / 1000.f;
        last_ticks = now;

        A += speed * dt * 0.7f;
        B += speed * dt * 0.3f;

        RenderDonut();
    }

    void DrawContent() override {
        ImGui::SetNextItemWidth(200); ImGui::SliderInt("Speed",     &speed,     1, 20);
        ImGui::SetNextItemWidth(200); ImGui::SliderInt("Size",      &size,      1, 12);
        ImGui::SetNextItemWidth(200); ImGui::SliderInt("Thickness", &thickness, 1,  6);
        ImGui::SetNextItemWidth(200); ImGui::SliderInt("Density",   &density,   1, 20);
        ImGui::Spacing();

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1
            ? ImGui::GetIO().Fonts->Fonts[1]
            : ImGui::GetIO().Fonts->Fonts[0]);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        float  fw     = ImGui::CalcTextSize("X").x;
        float  fh     = ImGui::GetTextLineHeight();
        float  vw     = fw * W;
        float  vh     = fh * H;

        ImGui::GetWindowDrawList()->AddRectFilled(
            cursor, ImVec2(cursor.x + vw, cursor.y + vh),
            IM_COL32(10, 10, 10, 230), 4.f);

        for (int row = 0; row < H; row++) {
            char line[W + 1];
            memcpy(line, screen + row * W, W);
            line[W] = '\0';
            ImGui::SetCursorScreenPos(ImVec2(cursor.x, cursor.y + row * fh));
            ImGui::TextColored(ImVec4(1.f, 0.55f, 0.1f, 1.f), "%s", line);
        }

        ImGui::SetCursorScreenPos(cursor);
        ImGui::InvisibleButton("##donut_active", ImVec2(vw, vh));
        ImGui::SetCursorScreenPos(ImVec2(cursor.x, cursor.y + vh));
        ImGui::Dummy(ImVec2(vw, 0));
        ImGui::PopFont();
        ImGui::Spacing();
    }

private:

    void RenderDonut() {
        int sz = std::clamp(size,      1, 12);
        int th = std::clamp(thickness, 1,  6);
        int dn = std::clamp(density,   1, 20);

        memset(screen, ' ', BUF);
        screen[BUF] = '\0';
        for (int i = 0; i < BUF; i++) zbuf[i] = 0.f;

        const float R1 = th * 0.5f;
        const float R2 = sz * 0.5f;
        const float K2 = 8.f;
        const float K1 = H * K2 * 3.f / (8.f * (R1 + R2));

        const float theta_step = 2.f * M_PI / (dn * 6);
        const float phi_step   = 2.f * M_PI / (dn * 12);

        const float sinA = sinf(A), cosA = cosf(A);
        const float sinB = sinf(B), cosB = cosf(B);

        for (float theta = 0; theta < 2.f * M_PI; theta += theta_step) {
            float cosT = cosf(theta), sinT = sinf(theta);
            float cx = R2 + R1 * cosT;
            float cy = R1 * sinT;

            for (float phi = 0; phi < 2.f * M_PI; phi += phi_step) {
                float cosP = cosf(phi), sinP = sinf(phi);

                float x   = cx * (cosB * cosP + sinA * sinB * sinP) - cy * cosA * sinB;
                float y   = cx * (sinB * cosP - sinA * cosB * sinP) + cy * cosA * cosB;
                float z   = K2 + cosA * cx * sinP + cy * sinA;
                float ooz = 1.f / z;

                int xp = (int)(W / 2.f + K1 * ooz * x);
                int yp = (int)(H / 2.f - K1 * ooz * y * 0.5f);

                if (xp < 0 || xp >= W || yp < 0 || yp >= H) continue;

                float L = cosP * cosT * sinB
                        - cosA * cosT * sinP
                        - sinA * sinT
                        + cosB * (cosA * sinT - cosT * sinA * sinP);

                if (L > 0.f && ooz > zbuf[yp * W + xp]) {
                    zbuf[yp * W + xp] = ooz;
                    int lumi = std::min((int)(L * 8.f), 11);
                    screen[yp * W + xp] = ".,-~:;=!*#$@"[lumi];
                }
            }
        }
    }
};