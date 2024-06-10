#include <Engine/Mono/Entity/Functions.hpp>
#include <string>
#include <iostream>
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components/CSharpScriptComponent.hpp"

namespace Engine::CSharpInternalFunctions {
    MonoObject *Entity_GetClassInstance(MonoString *ID, MonoString *type) {
        auto &gameObject = Scene::Main->FindGameObjectByID(mono_string_to_utf8(ID));
        const std::string m_type = mono_string_to_utf8(type);
        std::cout << "Tryna find shiii\n";
        if (gameObject == nullptr)
            return nullptr;
        
        std::cout << "found the good shi\n";

        auto &manager = gameObject->GetComponent<Components::CSharpScriptComponent>();
        std::cout << "size: " << manager.script_instances.size() << "\n";

        for (auto &script : manager.script_instances) {
            std::cout << "First: " << script.first << " m_type: " << m_type << "\n";
            if (script.first == "")
                continue;

            if (script.first.find(m_type) != std::string::npos) {
                auto &klass = script.second;
                return klass->GetHandleTarget();
            }
        }

        return NULL;
    }
}