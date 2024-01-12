#include <Engine/Components/CXXScriptComponent.hpp>
#include <Engine/Scene.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

using namespace Engine::Scripting::CXX;
namespace Engine::Components {
    void CXXScriptComponent::Init() {}

    void CXXScriptComponent::Update() {
        for (auto *script : script_instances) {
            script->OnUpdate();
        }
    }

    void CXXScriptComponent::OnStart() {
        for (auto *script : script_instances) {
            script->OnStart();
        }
    }

    void CXXScriptComponent::OnGUI() {
        DrawComponent<CXXScriptComponent>(Scene::Main->EntityRegistry, 40, [&] {
            static std::string input;
            ImGui::InputText("Script Filename", &go_go);
            bool d = ImGui::Button("Add Component");

            if (d && go_go != "" && go_go.ends_with(".cpp")) {
                CXXScripting::SharedObject &obj = CXXScripting::GetSharedObject(go_go);
                Script *script = obj.create_script();
                // script_instances.push_back();
                go_go = "";
            }
        });
    }
} // namespace Engine::Components