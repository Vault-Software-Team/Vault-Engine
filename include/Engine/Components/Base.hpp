#pragma once
#include <entt/entt.hpp>

namespace Engine {
    namespace Components {
        struct Base {
            entt::entity entity;
            std::string ID;

            virtual void OnGUI(){};
            virtual void Update() {}
            virtual void Init() {}
        };
    } // namespace Components
} // namespace Engine