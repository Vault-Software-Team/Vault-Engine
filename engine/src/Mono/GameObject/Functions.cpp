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
} // namespace Engine::CSharpInternalFunctions