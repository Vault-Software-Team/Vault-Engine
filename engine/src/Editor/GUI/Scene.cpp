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
        ImGui::Begin(ICON_FA_CUBES " Scene", NULL, ImGuiWindowFlags_NoScrollbar);
        ImVec2 size = ImGui::GetWindowSize();
        if (Window::window->use_imgui_size) {
            Window::window->width = size.x;
            Window::window->height = size.y;
            // glViewport(0, 0, size.x, size.y);
        }
        ImGui::Image((void *)framebufferTextureID, ImVec2(size.x, size.y - 35), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
    }
} // namespace Editor