#include "Engine/Components/Rigidbody2D.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "mono/metadata/object.h"
#include <Engine/Mono/Transform/Functions.hpp>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine::CSharpInternalFunctions {
    void Rigidbody2D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

        if (m_key == "velocity") {
            *result = CSharpHelper::StrToMonoString(std::to_string(((b2Body *)rigidbody.m_RuntimeBody)->GetLinearVelocity().x) + " " + std::to_string(((b2Body *)rigidbody.m_RuntimeBody)->GetLinearVelocity().y));
        }

        if (m_key == "type") {
            *result = CSharpHelper::StrToMonoString(std::to_string((int)rigidbody.body_type));
        }
    }
    void Rigidbody2D_SetVelocity(float x, float y, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

        rigidbody.SetVelocity(x, y);
    }
    void Rigidbody2D_SetAngularVelocity(float velocity, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

        rigidbody.SetAngularVelocity(velocity);
    }
    void Rigidbody2D_SetPosition(float x, float y, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

        rigidbody.SetPosition(x, y);
    }
    void Rigidbody2D_Force(float x, float y, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();

        rigidbody.Force(x, y);
    }
    void Rigidbody2D_Torque(float torque, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();
        rigidbody.Torque(torque);
    }

    void Rigidbody2D_SetType(int type, MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody2D>();
        rigidbody.body_type = (Components::Rigidbody2D::BodyType)type;
    }
} // namespace Engine::CSharpInternalFunctions