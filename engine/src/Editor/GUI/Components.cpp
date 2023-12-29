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

void AddComponentsGUIPopup(GameObject *gameObject) {
    if (ImGui::Button("Transform", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<Transform>()) {
            gameObject->AddComponent<Transform>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Ambient Light", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<AmbientLight>()) {
            gameObject->AddComponent<AmbientLight>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Directional Light", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<DirectionalLight>()) {
            gameObject->AddComponent<DirectionalLight>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Mesh Renderer", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<MeshRenderer>()) {
            gameObject->AddComponent<MeshRenderer>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Point Light", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<PointLight>()) {
            gameObject->AddComponent<PointLight>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Spot Light", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<SpotLight>()) {
            gameObject->AddComponent<SpotLight>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Text 3D", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<Text3D>()) {
            gameObject->AddComponent<Text3D>();
        }

        ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Camera", ImVec2(200, 0))) {
        if (!gameObject->HasComponent<Camera>()) {
            gameObject->AddComponent<Camera>();
        }

        ImGui::CloseCurrentPopup();
    }
}

namespace Editor {
    void GUI::Components() {
        constexpr int search_size = 256;
        static char name[search_size];
        static char search[search_size];

        std::string filterStr = search;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), asciitolower);

        ImGui::Begin(ICON_FA_SHARE_NODES " Components");
        ImVec2 win_size = ImGui::GetWindowSize();

        // Check if a GameObject is selected
        if (!GUI::selected_gameObject) {
            ImGui::End();
            return;
        }

        ImGui::InputText("Name", &selected_gameObject->name);
        ImGui::InputText("Tag", &selected_gameObject->tag);

        ComponentGUI(GUI::selected_gameObject);

        if (ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(win_size.x - 20, 0))) {
            ImGui::OpenPopup("Add Component");
        }

        if (ImGui::BeginPopup("Add Component")) {
            AddComponentsGUIPopup(GUI::selected_gameObject);
            ImGui::EndPopup();
        }

        ImGui::End();
    }
} // namespace Editor