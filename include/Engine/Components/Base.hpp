#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Base {
            entt::entity entity;
            std::string ID;
            static inline const std::string display_name = "Base Component";
            bool static_registry = false;

            virtual void OnGUI(){};
            virtual void Update() {}
            virtual void Init() {}
        };
    } // namespace Components
} // namespace Engine