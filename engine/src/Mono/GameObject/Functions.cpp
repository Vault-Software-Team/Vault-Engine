#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include "Engine/Mono/GameObject/Functions.hpp"
#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine::CSharpInternalFunctions {
    MonoString *GameObject_GetName(MonoString *ID) {
        auto gameObject = GameObject::FindGameObjectByID(CSharpHelper::MonoStrToString(ID));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->name);
    }
    MonoString *GameObject_GetTag(MonoString *ID) {
        auto gameObject = GameObject::FindGameObjectByID(CSharpHelper::MonoStrToString(ID));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->tag);
    }

    MonoString *GameObject_GetIDByName(MonoString *name) {
        auto gameObject = GameObject::FindGameObjectByName(CSharpHelper::MonoStrToString(name));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->ID);
    }
    MonoString *GameObject_GetIDByTag(MonoString *tag) {
        auto gameObject = GameObject::FindGameObjectByTag(CSharpHelper::MonoStrToString(tag));
        if (!gameObject) {
            return CSharpHelper::StrToMonoString("GAMEOBJECT_NOT_FOUND");
        }
        return CSharpHelper::StrToMonoString(gameObject->ID);
    }
} // namespace Engine::CSharpInternalFunctions