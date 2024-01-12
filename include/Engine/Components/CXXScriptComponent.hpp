#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Shader.hpp>
#include "Transform.hpp"
#include <Engine/Scripting/LoadScripts.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API CXXScriptComponent : Base {
            static inline const std::string display_name = "C++ Scripts";

            std::vector<Scripting::CXX::Script *> script_instances;
            std::string go_go;

            CXXScriptComponent() = default;
            void Init() override;
            void OnStart();
            void Update() override;
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine