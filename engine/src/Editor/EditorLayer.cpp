#include <Editor/GUI/MainGUI.hpp>
#include <Editor/EditorLayer.hpp>

namespace Editor {
    EditorLayer::EditorLayer() {
        instance = this;
    }

    void EditorLayer::GUI() {
        GUI::Hierarchy();
    }
} // namespace Editor