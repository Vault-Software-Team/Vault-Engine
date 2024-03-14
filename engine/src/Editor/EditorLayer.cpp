#include "imgui/imgui.h"
#include <imgui/ImGuiNotify.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <Editor/EditorLayer.hpp>
#include <Renderer/Stats.hpp>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <icons/icons.h>

namespace Editor {
    // Variable Definition
    DLL_API EditorLayer *EditorLayer::instance;

    EditorLayer::EditorLayer() {
        instance = this;

        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FILE);
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(1, 1, 1, 1.0f), ICON_FA_FOLDER);
    }

    void EditorLayer::GUI() {
        GUI::MenuBar();

        GUI::Hierarchy();
        GUI::Assets();
        GUI::Console();
        GUI::Scene();
        GUI::Components();
        GUI::MaterialInspector();
        GUI::BottomLogCounter();
        GUI::Config();
        GUI::DeveloperConsole();

        ImGui::Begin("Statistics");
        ImGui::Text("Draw Calls: %d", VaultRenderer::Statistics::GetDrawCalls());
        ImGui::Text("FPS: %d", VaultRenderer::Statistics::fps);
        ImGui::Text("Vendor: %s", VaultRenderer::Statistics::vendor.c_str());
        ImGui::Text("Renderer: %s", VaultRenderer::Statistics::renderer.c_str());
        ImGui::Text("Version: %s", VaultRenderer::Statistics::version.c_str());
        ImGui::Text("Shading Language: %s", VaultRenderer::Statistics::shading_language.c_str());
        ImGui::End();

        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);   // Disable round borders
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); // Disable borders

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.00f)); // Background color

        ImGui::RenderNotifications();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);
    }

    void EditorLayer::GameGUI() {
        GUI::Game_Scene();
    }
} // namespace Editor