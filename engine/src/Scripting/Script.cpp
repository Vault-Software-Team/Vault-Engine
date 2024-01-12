#pragma once
#include <Engine/Scripting/Script.hpp>

namespace Engine::Scripting::CXX {
    void Script::__VAULT_API_InitEngine(VAULT_API_EngineInit &engine) {
#ifdef EXPERIMENTAL_DLL
        Scene::SetMainScene(engine.main_scene);
#endif
    }
} // namespace Engine::Scripting::CXX