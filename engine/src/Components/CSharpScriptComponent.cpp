#include "Engine/Mono/CSharp.hpp"
#include <Engine/Components/CSharpScriptComponent.hpp>
#include <Engine/Scene.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui.h>
#include <memory>

namespace Engine::Components {
    void CSharpScriptComponent::Init() {}

    void CSharpScriptComponent::Update() {
        for (auto script : script_instances) {
            script.second->OnUpdate();
        }
    }

    void CSharpScriptComponent::OnStart() {
        for (auto script : script_instances) {
            script.second->OnStart(ID);
        }
    }

    void CSharpScriptComponent::OnGUI() {
        DrawComponent<CSharpScriptComponent>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::BeginListBox("C# Scripts");

            for (auto klass : CSharp::instance->entity_classes) {
                bool check = selected_scripts[klass.first] == klass.first;
                bool pressed = ImGui::Selectable(check ? (std::string(ICON_FA_CHECK) + " " + klass.first).c_str() : klass.first.c_str(), check);

                if (!pressed) continue; // Basically if not pressed just go to the next one duhh

                if (selected_scripts[klass.first] == klass.first) {
                    if (script_instances.find(klass.first) != script_instances.end()) {
                        script_instances.erase(script_instances.find(klass.first));
                    }

                    selected_scripts.erase(klass.first);
                } else {
                    script_instances[klass.first] = std::make_shared<ScriptClass>(CSharp::instance->core_assembly_image, klass.second.first, klass.second.second);
                    selected_scripts[klass.first] = klass.first;
                }
            }

            ImGui::EndListBox();

            ImGui::NewLine();
        });
    }
} // namespace Engine::Components