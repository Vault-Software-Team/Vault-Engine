#include "Renderer/Stats.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <Renderer/Window.hpp>
#include <ImGuizmo/ImGuizmo.h>
#include <Engine/SceneSerialization.hpp>
#include <Engine/Runtime.hpp>
#include <Engine/Audio.hpp>

using namespace Engine;
using namespace Engine::Components;
using namespace VaultRenderer;

namespace Editor {
    void GUI::Game_Scene() {
        static std::string before_serialized = "";

        if (!Runtime::instance)
            return;

        bool &isStopped = Runtime::instance->isStopped;
        bool &isRunning = Runtime::instance->isRunning;

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(0.0f, 0.0f);
        int width, height;
        glfwGetWindowSize(Window::window->GetGLFWWindow(), &width, &height);
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowBgAlpha(0);
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set position to the bottom of the viewport
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));

        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        if (ImGui::Begin("Game Scene", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking)) {
            ImVec2 size = ImGui::GetWindowSize();

            if (Window::window->use_imgui_size) {
                Window::window->width = size.x;
                Window::window->height = size.y;
                // glViewport(0, 0, size.x, size.y);
            }

            ImGui::Image((void *)framebufferTextureID, size, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::SetCursorPos(ImVec2(5, 5));
            ImGui::Text("FPS: %d", Statistics::fps);

            ImGui::End();
        }

        style.WindowPadding = ImVec2(8.0f, 8.0f);
    }
} // namespace Editor