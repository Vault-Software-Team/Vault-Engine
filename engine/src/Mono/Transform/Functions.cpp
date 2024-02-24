#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "mono/metadata/object.h"
#include <Engine/Mono/Transform/Functions.hpp>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>

namespace Engine::CSharpInternalFunctions {
    void Transform_GetPosition(MonoString *ID, MonoString **out) {
        const std::string id = CSharpHelper::MonoStrToString(ID);

        auto gameObject = GameObject::FindGameObjectByID(id);

        if (!gameObject) {
            *out = CSharpHelper::StrToMonoString("0 0 0");
            std::cout << "C# Error: GameObject not found!\n";
            return;
        }

        auto &transform = gameObject->GetComponent<Components::Transform>();
        *out = CSharpHelper::StrToMonoString(std::to_string(transform.position.x) + " " + std::to_string(transform.position.y) + " " + std::to_string(transform.position.z));
    }

    void Transform_GetRotation(MonoString *ID, MonoString **out) {
        const std::string id = CSharpHelper::MonoStrToString(ID);

        auto gameObject = GameObject::FindGameObjectByID(id);

        if (!gameObject) {
            *out = CSharpHelper::StrToMonoString("0 0 0");
            std::cout << "C# Error: GameObject not found!\n";
            return;
        }

        auto &transform = gameObject->GetComponent<Components::Transform>();
        *out = CSharpHelper::StrToMonoString(std::to_string(transform.rotation.x) + " " + std::to_string(transform.rotation.y) + " " + std::to_string(transform.rotation.z));
    }

    void Transform_GetScale(MonoString *ID, MonoString **out) {
        const std::string id = CSharpHelper::MonoStrToString(ID);

        auto gameObject = GameObject::FindGameObjectByID(id);

        if (!gameObject) {
            *out = CSharpHelper::StrToMonoString("0 0 0");
            std::cout << "C# Error: GameObject not found!\n";
            return;
        }

        auto &transform = gameObject->GetComponent<Components::Transform>();
        *out = CSharpHelper::StrToMonoString(std::to_string(transform.scale.x) + " " + std::to_string(transform.scale.y) + " " + std::to_string(transform.scale.z));
    }

    void Transform_SetField(MonoString *ID, MonoString *key, float x, float y, float z) {
        const std::string id = CSharpHelper::MonoStrToString(ID);
        const std::string _key = CSharpHelper::MonoStrToString(key);
        auto gameObject = GameObject::FindGameObjectByID(id);

        if (!gameObject) {
            std::cout << "C# Error: GameObject not found!\n";
            return;
        }

        auto &transform = gameObject->GetComponent<Components::Transform>();

        if (_key == "position") {
            transform.position.x = x;
            transform.position.y = y;
            transform.position.z = z;
        }

        if (_key == "rotation") {
            transform.rotation.x = x;
            transform.rotation.y = y;
            transform.rotation.z = z;
        }

        if (_key == "scale") {
            transform.scale.x = x;
            transform.scale.y = y;
            transform.scale.z = z;
        }
    }
} // namespace Engine::CSharpInternalFunctions