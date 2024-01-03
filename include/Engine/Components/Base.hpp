#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Base {
            entt::entity entity;
            std::string ID;
            bool static_registry = false;

            virtual void OnGUI(){};
            virtual void Update() {}
            virtual void Init() {}
        };
    } // namespace Components
} // namespace Engine