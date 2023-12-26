#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Renderer/Window.hpp>

using namespace Engine;
using namespace Engine::Components;
using namespace VaultRenderer;

namespace Editor {
    void GUI::Scene() {
        ImGui::Begin(ICON_FA_CUBES " Scene");
        std::string icon = ICON_FA_CUBE;

        if (Window::window->use_imgui_size) {
            Window::window->width = ImGui::GetWindowSize().x;
            Window::window->height = ImGui::GetWindowSize().y;
            // glViewport(0, 0, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
        }
        ImGui::Image((void *)framebufferTextureID, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
    }
} // namespace Editor