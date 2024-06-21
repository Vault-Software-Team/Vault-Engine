#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include "Transform.hpp"
#include <Engine/Physics/BulletPhysics.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Rigidbody3D : Base {
            static inline const std::string display_name = "Rigidbody 3D";
            Transform *transform;

            float mass = 1;
            float restitution = 0.0f;
            float friction = 0.5f;
            bool trigger = false;
            bool fixedRotation = false;

            btRigidBody *body = nullptr;
            btDefaultMotionState *motionState = nullptr;
            btTransform *bt_transform = nullptr;
            btCollisionShape *ref;

            btVector3 inertia = btVector3(0, 0, 0);
            Rigidbody3D() = default;
            void Init() override;
            void OnGUI() override;
            void Update();

            void CreateBody(btCollisionShape *shape);

            void AddForce(const glm::vec3 &force);
            void AddTorque(const glm::vec3 &torque);
            void AddForceAtPosition(const glm::vec3 &force, const glm::vec3 &position);

            // velocity functions
            void SetVelocity(const glm::vec3 &velocity);
            void SetAngularVelocity(const glm::vec3 &velocity);
            glm::vec3 GetVelocity();
            glm::vec3 GetAngularVelocity();
        };
    } // namespace Components
} // namespace Engine