#include <Editor/GUI/MainGUI.hpp>
#include <Editor/EditorLayer.hpp>

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
    }
} // namespace Editor