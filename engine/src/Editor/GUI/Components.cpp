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

void ComponentGUI(GameObject *gameObject) {
    if (gameObject->HasComponent<Transform>()) {
        gameObject->GetComponent<Transform>().OnGUI();
    }

    if (gameObject->HasComponent<AmbientLight>()) {
        gameObject->GetComponent<AmbientLight>().OnGUI();
    }

    if (gameObject->HasComponent<DirectionalLight>()) {
        gameObject->GetComponent<DirectionalLight>().OnGUI();
    }

    if (gameObject->HasComponent<MeshRenderer>()) {
        gameObject->GetComponent<MeshRenderer>().OnGUI();
    }

    if (gameObject->HasComponent<PointLight>()) {
        gameObject->GetComponent<PointLight>().OnGUI();
    }

    if (gameObject->HasComponent<SpotLight>()) {
        gameObject->GetComponent<SpotLight>().OnGUI();
    }

    if (gameObject->HasComponent<Text3D>()) {
        gameObject->GetComponent<Text3D>().OnGUI();
    }

    if (gameObject->HasComponent<Camera>()) {
        gameObject->GetComponent<Camera>().OnGUI();
    }
}

namespace Editor {
    void GUI::Components() {
        constexpr int search_size = 256;
        static char name[search_size];
        static char search[search_size];

        ImVec2 win_size = ImGui::GetWindowSize();

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

        ComponentGUI(GUI::selected_gameObject);

        if (ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(win_size.x - 15, 0))) {
            ImGui::OpenPopup("Add Component");
        }

        ImGui::End();
    }
} // namespace Editor