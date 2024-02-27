#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API Rigidbody2D : Base {
            static inline const std::string display_name = "Rigidbody 2D";
            Transform *transform;

            enum class BodyType {
                STATIC = 0,
                KINEMATIC,
                DYNAMIC,
            };
            BodyType body_type = BodyType::STATIC;
            bool fixed_rotation = false;
            void *m_RuntimeBody = nullptr;
            float gravity_scale = 1.0;

            Rigidbody2D() = default;
            void Init() override;
            void OnGUI() override;

            void SetVelocity(float x, float y);
            void SetAngularVelocity(float velocity);
            void SetPosition(float x, float y);
            void Force(float x, float y);
            void Torque(float torque);
        };
    } // namespace Components
} // namespace Engine