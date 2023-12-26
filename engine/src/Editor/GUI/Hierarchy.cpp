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
        ImGui::Begin("Hierarhcy");
        std::string icon = ICON_FA_CUBE;

        for (auto &gameObject : Scene::Main->GameObjects) {
            bool hasChildren = false;
            for (auto &gameObject : Scene::Main->GameObjects) {
                if (gameObject->parent == gameObject->ID) {
                    hasChildren = true;
                    break;
                }
            }
            SetNameIcon(icon, gameObject);

            ImGui::Selectable((icon + " " + gameObject->name).c_str());
        }

        ImGui::End();
    }
} // namespace Editor