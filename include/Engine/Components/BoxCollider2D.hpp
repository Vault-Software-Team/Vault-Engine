#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API BoxCollider2D : Base {
            static inline const std::string display_name = "Box Collider 2D";
            Transform *transform;

            glm::vec2 offset{0, 0};
            glm::vec2 size{0.5, 0.5};

            float density = 1;
            float friction = 1;
            float restitution = 0;
            bool trigger = false;
            // float restitution_threshold = 0.5;

            BoxCollider2D() = default;
            void Init() override;
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine