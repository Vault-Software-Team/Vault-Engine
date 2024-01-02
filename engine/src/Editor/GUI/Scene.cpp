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
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(0.0f, 0.0f);
        ImGui::Begin(ICON_FA_CUBES " Scene", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImVec2 size = ImGui::GetWindowSize();
        if (Window::window->use_imgui_size) {
            Window::window->width = size.x;
            Window::window->height = size.y;
            // glViewport(0, 0, size.x, size.y);
        }

        ImGui::Image((void *)framebufferTextureID, size, ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsWindowHovered()) {
            if (glfwGetMouseButton(Window::window->GetGLFWWindow(), GLFW_MOUSE_BUTTON_2)) {
                Scene::EditorSceneCamera->Inputs();
            }
        }

        ImGui::End();
        style.WindowPadding = ImVec2(8.0f, 8.0f);
    }
} // namespace Editor