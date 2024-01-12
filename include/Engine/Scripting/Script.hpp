#pragma once
#include <Engine/Scene.hpp>
#include <memory>

namespace Engine::Scripting::CXX {
    struct VAULT_API_EngineInit {
        std::shared_ptr<Scene> main_scene;
    };

    class Script {
    public:
        virtual void OnStart();
        virtual void OnUpdate();

        void __VAULT_API_InitEngine(VAULT_API_EngineInit &engine);
    };
} // namespace Engine::Scripting::CXX