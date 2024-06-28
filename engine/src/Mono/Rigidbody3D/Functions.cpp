#include "Engine/Components/Rigidbody3D.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "mono/metadata/object.h"
#include <Engine/Mono/Transform/Functions.hpp>
#include <Engine/Mono/Rigidbody3D/Functions.hpp>
#include <cstdlib>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine::CSharpInternalFunctions {
    void Rigidbody3D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();

        if (m_key == "velocity") {
            *result = CSharpHelper::StrToMonoString(std::to_string(rigidbody.GetVelocity().x) + " " + std::to_string(rigidbody.GetVelocity().y) + " " + std::to_string(rigidbody.GetVelocity().z));
        }

        if (m_key == "mass") {
            *result = CSharpHelper::StrToMonoString(std::to_string(rigidbody.mass));
        }

        if (m_key == "restitution") {
            *result = CSharpHelper::StrToMonoString(std::to_string(rigidbody.restitution));
        }

        if (m_key == "trigger") {
            *result = CSharpHelper::StrToMonoString(rigidbody.trigger ? "true" : "false");
        }

        if (m_key == "fixedRotation") {
            *result = CSharpHelper::StrToMonoString(rigidbody.fixedRotation ? "true" : "false");
        }
    }

    void Rigidbody3D_SetKey(MonoString *key, MonoString *id, MonoString *value) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        const std::string m_value = mono_string_to_utf8(value);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();

        if (m_key == "velocity") {
            double num1, num2, num3;
            std::istringstream stream(m_value);
            stream >> num1 >> num2 >> num3;

            rigidbody.SetVelocity(glm::vec3(num1, num2, num3));
        }

        if (m_key == "mass") {
            rigidbody.mass = std::atof(m_value.c_str());
        }

        if (m_key == "restitution") {
            rigidbody.restitution = std::atof(m_value.c_str());
        }

        if (m_key == "trigger") {
            rigidbody.trigger = m_value == "true";
        }

        if (m_key == "fixedRotation") {
            rigidbody.fixedRotation = m_value == "true";
        }
    }

    void Rigidbody3D_AddForce(MonoString *id, float x, float y, float z) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();
        rigidbody.AddForce(glm::vec3(x, y, z));
    }

    void Rigidbody3D_AddTorque(MonoString *id, float x, float y, float z) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();
        rigidbody.AddTorque(glm::vec3(x, y, z));
    }

    void Rigidbody3D_AddForceAtPosition(MonoString *id, float fx, float fy, float fz, float px, float py, float pz) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();
        rigidbody.AddForceAtPosition(glm::vec3(fx, fy, fz), glm::vec3(px, py, pz));
    }

    void Rigidbody3D_SetVelocity(MonoString *id, float x, float y, float z) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();
        rigidbody.SetVelocity(glm::vec3(x, y, z));
    }

    void Rigidbody3D_SetAngularVelocity(MonoString *id, float x, float y, float z) {
        const std::string m_id = mono_string_to_utf8(id);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Components::Rigidbody3D>();
        rigidbody.SetAngularVelocity(glm::vec3(x, y, z));
    }
} // namespace Engine::CSharpInternalFunctions