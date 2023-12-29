#include <Editor/GUI/MainGUI.hpp>
#include <Editor/EditorLayer.hpp>
#include <Renderer/Stats.hpp>

namespace Editor {
    // Variable Definition
    DLL_API EditorLayer *EditorLayer::instance;

    EditorLayer::EditorLayer() {
        instance = this;
    }

    void EditorLayer::GUI() {
        GUI::Hierarchy();
        GUI::Assets();
        GUI::Console();
        GUI::Scene();
        GUI::Components();

        ImGui::Begin("Statistics");
        ImGui::Text("Draw Calls: %d", VaultRenderer::Statistics::GetDrawCalls());
        ImGui::Text("Vendor: %s", VaultRenderer::Statistics::vendor.c_str());
        ImGui::Text("Renderer: %s", VaultRenderer::Statistics::renderer.c_str());
        ImGui::Text("Version: %s", VaultRenderer::Statistics::version.c_str());
        ImGui::Text("Shading Language: %s", VaultRenderer::Statistics::shading_language.c_str());
        ImGui::End();
    }
} // namespace Editor