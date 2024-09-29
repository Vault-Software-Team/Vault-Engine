#include "Engine/Components/ModelAnimator.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "mono/metadata/object.h"
#include <Engine/Mono/ModelAnimator/Functions.hpp>
#include <cstdlib>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine::CSharpInternalFunctions {
    MonoString *ModelAnimator_GetCurrentAnimation(MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return CSharpHelper::StrToMonoString("");
        }

        return CSharpHelper::StrToMonoString(gameObject->GetComponent<Components::ModelAnimator>().current_animation);
    }

    void ModelAnimator_SetCurrentAnimation(MonoString *id, MonoString *value) {
        const std::string m_id = mono_string_to_utf8(id);
        const std::string m_value = mono_string_to_utf8(value);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        gameObject->GetComponent<Components::ModelAnimator>().current_animation = m_value;
    }
} // namespace Engine::CSharpInternalFunctions