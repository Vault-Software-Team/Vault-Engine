#include "Engine/Mono/CSharp.hpp"
#include "mono/metadata/class.h"
#include <Engine/Components/CSharpScriptComponent.hpp>
#include <Engine/Scene.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui.h>
#include <memory>

namespace Engine::Components {
    void CSharpScriptComponent::Init() {
        for (auto klass : selected_scripts) {
            std::cout << klass.second.first << "." << klass.second.second << "\n";
            script_instances[klass.first] = std::make_shared<ScriptClass>(CSharp::instance->core_assembly_image, klass.second.first, klass.second.second);
            script_instances[klass.first]->InitInstance(ID);
        }
    }

    void CSharpScriptComponent::Update() {
        for (auto script : script_instances) {
            script.second->OnUpdate();
        }
    }

    void CSharpScriptComponent::OnStart() {
        std::cout << "Initing script component! " << ID << "\n";
        for (auto klass : selected_scripts) {
            std::cout << klass.second.first << "." << klass.second.second << "\n";
            // script_instances[klass.first] = std::make_shared<ScriptClass>(CSharp::instance->core_assembly_image, klass.second.first, klass.second.second);
            script_instances[klass.first]->OnStart(ID);
        }
    }

    void CSharpScriptComponent::OnGUI() {
        DrawComponent<CSharpScriptComponent>(Scene::Main->EntityRegistry, 40, [&] {
            for (auto klass : CSharp::instance->entity_classes) {
                if (ImGui::TreeNode(klass.first.c_str())) {
                    auto _ = selected_scripts.find(klass.first);
                    auto found = _ != selected_scripts.end() ? _->first : "";

                    bool check = found == klass.first;
                    bool pressed = ImGui::Selectable(check ? (std::string(ICON_FA_CHECK) + " Selected").c_str() : "Press to select", check);

                    if (!pressed) {
                        ImGui::TreePop();
                        continue;
                    }; // Basically if not pressed just go to the next one duhh

                    if (check) {
                        if (script_instances.find(klass.first) != script_instances.end()) {
                            script_instances.erase(script_instances.find(klass.first));
                        }

                        selected_scripts.erase(klass.first);
                    } else {
                        // script_instances[klass.first] = std::make_shared<ScriptClass>(CSharp::instance->core_assembly_image, klass.second.first, klass.second.second);
                        selected_scripts[klass.first] = klass.second;
                    }

                    void *iterator = nullptr;
                    MonoClass *entity_class = mono_class_from_name(CSharp::instance->core_assembly_image, klass.second.first.c_str(), klass.second.second.c_str());
                    while (MonoClassField *field = mono_class_get_fields(entity_class, &iterator)) {
                        const char *field_name = mono_field_get_name(field);
                        MonoType *type = mono_field_get_type(field);
                        const char *type_name = mono_type_get_name(type);
                        std::cout << type_name << " " << field_name << "\n";
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::NewLine();
        });
    }
} // namespace Engine::Components