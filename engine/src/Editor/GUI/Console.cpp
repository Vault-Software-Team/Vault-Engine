#include "Engine/Runtime.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>

using namespace Engine;
using namespace Engine::Components;

namespace Editor {
    void GUI::LogInfo(const std::string &content) {
        logs.push_back({Log::LOG_INFO, content});
    }
    void GUI::LogTick(const std::string &content) {
        logs.push_back({Log::LOG_TICK, content});
    }
    void GUI::LogError(const std::string &content) {
        logs.push_back({Log::LOG_ERROR, content});
    }
    void GUI::LogWarning(const std::string &content) {
        logs.push_back({Log::LOG_WARNING, content});
    }

    void GUI::Console() {
        ImGui::Begin(ICON_FA_TERMINAL " Console");
        if (ImGui::Button(ICON_FA_TRASH_CAN " Clear")) {
            logs.clear();
        }
        ImGui::Separator();

        for (auto &log : logs) {
            switch (log.type) {
            case Log::LOG_INFO:
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                                   ICON_FA_MESSAGE);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string("  ") + log.content).c_str());
                break;
            case Log::LOG_WARNING:
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f),
                                   ICON_FA_TRIANGLE_EXCLAMATION);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string("  ") + log.content).c_str());
                break;
            case Log::LOG_ERROR:
                ImGui::TextColored(ImVec4(1.2f, 0.0f, 0.0f, 1.0f),
                                   ICON_FA_CIRCLE_EXCLAMATION);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string("  ") + log.content).c_str());
                break;
            case Log::LOG_TICK:
                ImGui::TextColored(ImVec4(0.019f, 0.988f, 0.08627f, 1),
                                   ICON_FA_CIRCLE_CHECK);
                // same line
                ImGui::SameLine();
                ImGui::TextWrapped("%s",
                                   (std::string("  ") + log.content).c_str());
                break;
            }
        }

        if (Runtime::instance->isRunning) {
            ImGui::SetScrollHereY(0.999f);
        }

        ImGui::End();
    }

    void GUI::BottomLogCounter() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set position to the bottom of the viewport
        ImGui::SetNextWindowPos(
            ImVec2(viewport->Pos.x,
                   (viewport->Pos.y + viewport->Size.y - ImGui::GetFrameHeight()) - 10));

        // Extend width to viewport width
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, ImGui::GetFrameHeight()));
        ImGui::SetNextWindowViewport(viewport->ID);

        // Add menu bar flag and disable everything else
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 0.5f));
        if (ImGui::Begin("StatusBar", nullptr, flags)) {
            int errors = 0, warnings = 0, infos = 0;

            for (auto log : GUI::logs) {
                switch (log.type) {
                case Log::LOG_ERROR:
                    errors++;
                    break;
                case Log::LOG_INFO:
                    infos++;
                    break;
                case Log::LOG_WARNING:
                    warnings++;
                    break;
                }
            }

            ImGui::TextColored(ImVec4(1.2f, 0.0f, 0.0f, 1.0f), "%s %d", ICON_FA_CIRCLE_EXCLAMATION, errors);
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "%s %d", ICON_FA_TRIANGLE_EXCLAMATION, warnings);

            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 0.635f, 1.0f, 1.0f), "%s %d", ICON_FA_CIRCLE_INFO, infos);

            ImGui::End();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
} // namespace Editor