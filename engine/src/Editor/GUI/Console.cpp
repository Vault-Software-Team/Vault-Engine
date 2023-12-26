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
            }
        }

        ImGui::End();
    }
} // namespace Editor