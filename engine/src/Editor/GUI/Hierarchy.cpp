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
        constexpr int search_size = 256;
        static char search[search_size];

        std::string filterStr = search;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), asciitolower);

        ImGui::Begin(ICON_FA_CUBES " Hierarhcy");
        if (ImGui::Button(ICON_FA_PLUS " Add GameObject", ImVec2(ImGui::GetWindowSize().x - 20, 25))) {
            auto &gameObject = GameObject::New("GameObject");
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();

        ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20);
        ImGui::InputText("##HierarhcySearch", search, search_size);

        for (auto &gameObject : Scene::Main->GameObjects) {
            if (gameObject->parent != "NO_PARENT")
                continue;

            std::string n = gameObject->name;
            std::transform(n.begin(), n.end(), n.begin(), asciitolower);
            if (n.find(filterStr) == std::string::npos && filterStr != "")
                continue;

            gameObject->GUI();
        }

        ImGui::End();
    }
} // namespace Editor