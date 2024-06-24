#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Engine/Components/ModelRenderer.hpp"
#include "Renderer/Mesh.hpp"
#include "Transform.hpp"
#include <Engine/Physics/BulletPhysics.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API ModelRigidbody3D : Base {
            static inline const std::string display_name = "Rigidbody 3D";
            Transform *transform;

            float mass = 1;
            float restitution = 0.0f;
            float friction = 0.5f;
            bool trigger = false;
            bool fixedRotation = false;

            struct MRBody {
                std::string id;
                std::string name;
                std::shared_ptr<btRigidBody> body = nullptr;
                std::shared_ptr<btDefaultMotionState> motionState = nullptr;
                std::shared_ptr<btTransform> bt_transform = nullptr;
                std::shared_ptr<btCollisionShape> ref;
                glm::vec3 localScale;
                VaultRenderer::Mesh *meshRef;
            };

            std::unordered_map<VaultRenderer::Mesh *, std::shared_ptr<MRBody>> rigidbodies;

            btRigidBody *body = nullptr;
            btDefaultMotionState *motionState = nullptr;
            btTransform *bt_transform = nullptr;
            btCollisionShape *ref;
            ModelRenderer *renderer;

            btVector3 inertia = btVector3(0, 0, 0);
            ModelRigidbody3D() = default;
            void Init() override;
            void OnGUI() override;
            void Update();

            void CreateBodiesFromMeshes();

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