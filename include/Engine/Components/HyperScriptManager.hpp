#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"
#include <HyperScript/HyperScript.hpp>
#include "Transform.hpp"
#include <map>

namespace Engine {
    namespace Components {
        struct DLL_API HyperScriptManager : Base {
            static inline const std::string display_name = "HyperScript Script Manager";
            Transform *transform;

            std::map<std::string, std::shared_ptr<HyperScript::ScriptEngine>> loaded_scripts;

            HyperScriptManager() = default;

            void Scripts_OnStart();
            void Init() override;
            void Update() override;
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine