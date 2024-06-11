#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/CSharp.hpp"
#include "mono/metadata/object.h"
#include <Engine/Mono/Camera/Functions.hpp>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine::CSharpInternalFunctions {
    float Camera_GetKey(MonoString *key, MonoString *id) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return 0;
        }

        auto &camera = gameObject->GetComponent<Components::Camera>();

        if (m_key == "FOV") {
            return camera.fov;
        }

        if (m_key == "far") {
            return camera.far;
        }

        if (m_key == "near") {
            return camera.near;
        }
    }

    void Camera_SetKey(MonoString *key, MonoString *id, float value) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &camera = gameObject->GetComponent<Components::Camera>();

        if (m_key == "FOV") {
            camera.fov = value;
        }

        if (m_key == "far") {
            camera.far = value;
        }

        if (m_key == "near") {
            camera.near = value;
        }
    }

    MonoString *Camera_GetFront(MonoString *id) {
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return CSharpHelper::StrToMonoString("0 0 0");
        }

        auto &camera = gameObject->GetComponent<Components::Camera>();
        return CSharpHelper::StrToMonoString(CSharpHelper::Vec3ToStr(camera.front));
    }

    bool Camera_GetBool(MonoString *key, MonoString *id) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return 0;
        }

        auto &camera = gameObject->GetComponent<Components::Camera>();

        if (m_key == "is2D") {
            return camera.is2D;
        }

        if (m_key == "mainCamera") {
            return camera.main_camera;
        }

        if (m_key == "depthCamera") {
            return camera.depth_camera;
        }
    }

    void Camera_SetBool(MonoString *key, MonoString *id, bool value) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &camera = gameObject->GetComponent<Components::Camera>();

        if (m_key == "is2D") {
            camera.is2D = value;
        }

        if (m_key == "mainCamera") {
            camera.main_camera = value;
        }

        if (m_key == "depthCamera") {
            camera.depth_camera = value;
        }
    }
} // namespace Engine::CSharpInternalFunctions