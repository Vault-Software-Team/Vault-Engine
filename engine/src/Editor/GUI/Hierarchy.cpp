#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>

using namespace Engine;
using namespace Engine::Components;

namespace Editor {
    void GUI::Hierarchy() {
        ImGui::Begin(ICON_FA_CUBES " Hierarhcy");
        std::string icon = ICON_FA_CUBE;

        for (auto &gameObject : Scene::Main->GameObjects) {
            gameObject->GUI();
        }

        ImGui::End();
    }
} // namespace Editor