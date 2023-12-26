#pragma once
#include <entt/entt.hpp>
#include <dllapi.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Base {
            entt::entity entity;
            std::string ID;

            virtual void OnGUI(){};
            virtual void Update() {}
            virtual void Init() {}
        };
    } // namespace Components
} // namespace Engine