#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"
#include <Engine/Mono/CSharp.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API CSharpScriptComponent : Base {
            static inline const std::string display_name = "C# Scripts";

            std::map<std::string, std::shared_ptr<ScriptClass>> script_instances;
            std::unordered_map<std::string, std::pair<std::string, std::string>> selected_scripts;

            CSharpScriptComponent() = default;
            void Init() override;
            void OnStart();
            void Update() override;
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine