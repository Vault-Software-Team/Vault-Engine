#pragma once

#include "Editor/GUI/MainGUI.hpp"
#include "Engine/Components/PointLight.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Mono/HelperFunctions.hpp"
#include <Engine/Mono/PointLight/Functions.hpp>
#include <string>

namespace Engine::CSharpInternalFunctions {
    std::vector<std::string> split(std::string s, std::string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    MonoString *PointLight_GetKey(MonoString *key, MonoString *id) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        std::string toSet = "";

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return 0;
        }

        auto &component = gameObject->GetComponent<Components::PointLight>();

        if (m_key == "intensity") {
            return CSharpHelper::StrToMonoString(std::to_string(component.intensity));
        }

        if (m_key == "shadowFarPlane") {
            return CSharpHelper::StrToMonoString(std::to_string(component.shadow_far_plane));
        }

        if (m_key == "enableShadowMapping") {
            return CSharpHelper::StrToMonoString(component.enable_shadow_mapping ? "true" : "false");
        }

        if (m_key == "color") {
            return CSharpHelper::StrToMonoString(
                std::to_string(component.color.x) + " " +
                std::to_string(component.color.y) + " " +
                std::to_string(component.color.z) + " " //
            );
        }

        return CSharpHelper::StrToMonoString("0 0 0");
    }
    void PointLight_SetKey(MonoString *key, MonoString *id, MonoString *value) {
        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);
        const std::string m_value = mono_string_to_utf8(value);

        auto &gameObject = GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Editor::GUI::LogError(("C#: Couldn't find game object with ID: " + m_id));
            return;
        }

        auto &component = gameObject->GetComponent<Components::PointLight>();

        if (m_key == "intensity") {
            component.intensity = std::stof(m_value);
        }

        if (m_key == "shadowFarPlane") {
            component.shadow_far_plane = std::stof(m_value);
        }

        if (m_key == "enableShadowMapping") {
            component.enable_shadow_mapping = m_value == "true";
        }

        if (m_key == "color") {
            std::vector<std::string> v = split(m_value, " ");
            component.color.x = std::stof(v[0]);
            component.color.y = std::stof(v[1]);
            component.color.z = std::stof(v[2]);
        }
    }
} // namespace Engine::CSharpInternalFunctions