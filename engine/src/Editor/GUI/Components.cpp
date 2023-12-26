#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

using namespace Engine;
using namespace Engine::Components;
// ICON_FA_SHARE_NODES " Components"
namespace Editor {
    void GUI::Components() {
        constexpr int search_size = 256;
        static char name[search_size];
        static char search[search_size];

        std::string filterStr = search;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), asciitolower);

        ImGui::Begin(ICON_FA_SHARE_NODES " Components");

        // Check if a GameObject is selected
        if (!GUI::selected_gameObject) {
            ImGui::End();
            return;
        }

        ImGui::InputText("Name", &selected_gameObject->name);
        ImGui::InputText("Tag", &selected_gameObject->tag);

        ImGui::End();
    }
} // namespace Editor