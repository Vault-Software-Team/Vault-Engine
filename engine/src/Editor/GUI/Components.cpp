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

template <typename T>
void CallComponentGUI(GameObject *gameObject) {
    if (gameObject->HasComponent<T>()) {
        gameObject->GetComponent<T>().OnGUI();
    }
}

void ComponentGUI(GameObject *gameObject) {
    CallComponentGUI<Transform>(gameObject);
    CallComponentGUI<AmbientLight>(gameObject);
    CallComponentGUI<DirectionalLight>(gameObject);
    CallComponentGUI<MeshRenderer>(gameObject);
    CallComponentGUI<ModelRenderer>(gameObject);
    CallComponentGUI<SpriteRenderer>(gameObject);
    CallComponentGUI<SpritesheetRenderer>(gameObject);
    CallComponentGUI<Rigidbody2D>(gameObject);
    CallComponentGUI<BoxCollider2D>(gameObject);
    CallComponentGUI<PointLight>(gameObject);
    CallComponentGUI<SpotLight>(gameObject);
    CallComponentGUI<Text3D>(gameObject);
    CallComponentGUI<Camera>(gameObject);
    CallComponentGUI<CSharpScriptComponent>(gameObject);
    // CallComponentGUI<HyperScriptManager>(gameObject);
}

template <typename T>
void DisplayAddComponentButton(GameObject *gameObject) {
    if (ImGui::Button(T::display_name.c_str(), ImVec2(200, 0))) {
        if (!gameObject->HasComponent<T>()) {
            gameObject->AddComponent<T>();
        }

        ImGui::CloseCurrentPopup();
    }
}

void AddComponentsGUIPopup(GameObject *gameObject) {
    DisplayAddComponentButton<Transform>(gameObject);
    DisplayAddComponentButton<AmbientLight>(gameObject);
    DisplayAddComponentButton<DirectionalLight>(gameObject);
    DisplayAddComponentButton<MeshRenderer>(gameObject);
    DisplayAddComponentButton<ModelRenderer>(gameObject);
    DisplayAddComponentButton<SpriteRenderer>(gameObject);
    DisplayAddComponentButton<SpritesheetRenderer>(gameObject);
    DisplayAddComponentButton<Rigidbody2D>(gameObject);
    DisplayAddComponentButton<BoxCollider2D>(gameObject);
    DisplayAddComponentButton<PointLight>(gameObject);
    DisplayAddComponentButton<SpotLight>(gameObject);
    DisplayAddComponentButton<Text3D>(gameObject);
    DisplayAddComponentButton<Camera>(gameObject);
    DisplayAddComponentButton<CSharpScriptComponent>(gameObject);
    // DisplayAddComponentButton<HyperScriptManager>(gameObject);
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
        if (ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(win_size.x - 20, 0))) {
            ImGui::OpenPopup("Add Component");
        }

        ComponentGUI(GUI::selected_gameObject);

        if (ImGui::BeginPopup("Add Component")) {
            AddComponentsGUIPopup(GUI::selected_gameObject);
            ImGui::EndPopup();
        }

        ImGui::End();
    }
} // namespace Editor