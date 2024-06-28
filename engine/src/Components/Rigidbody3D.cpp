#include "Engine/DecomposeTransform.hpp"
#include "Engine/Physics/BulletPhysics.hpp"
#include "Engine/Runtime.hpp"
#include <Engine/Components/Rigidbody3D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void Rigidbody3D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void Rigidbody3D::OnGUI() {
        DrawComponent<Rigidbody3D>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Mass", &mass, 0.01f);
            ImGui::DragFloat("Friction", &friction, 0.01f);
            ImGui::DragFloat("Restitution", &restitution, 0.01f);
            ImGui::Checkbox("Trigger", &trigger);
            ImGui::Checkbox("Fixed Rotation", &fixedRotation);
        });
    }

    void Rigidbody3D::CreateBody(btCollisionShape *shape) {
        ref = shape;
        bt_transform = new btTransform();
        bt_transform->setIdentity();
        bt_transform->setOrigin(btVector3(transform->position.x,
                                          transform->position.y,
                                          transform->position.z));
        glm::vec3 q = transform->rotation;
        btQuaternion rotation;
        rotation.setEulerZYX(q.z, q.y, q.x);
        std::cout << "Created a Bulet Physics Rigidbody\n";

        bt_transform->setRotation(rotation);

        motionState = new btDefaultMotionState(*bt_transform);

        if (mass > 0) {
            shape->calculateLocalInertia(mass, inertia);
        }

        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState,
                                                        shape, inertia);
        rbInfo.m_restitution = restitution;
        rbInfo.m_friction = friction;
        body = new btRigidBody(rbInfo);
        // set user data
        body->setUserPointer(&ID);
        // set trigger
        body->setCollisionFlags(
            body->getCollisionFlags() |
            btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        if (trigger) {
            body->setCollisionFlags(
                body->getCollisionFlags() |
                btCollisionObject::CO_GHOST_OBJECT);

            body->setCollisionFlags(
                body->getCollisionFlags() |
                btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

            body = (btRigidBody *)(new btGhostObject());
        }

        if (fixedRotation) {
            body->setAngularFactor(btVector3(0, 0, 0));
        }

        Physics3D::instance->dynamicsWorld->addRigidBody(body, 1, 1);
    }

    void Rigidbody3D::AddForce(const glm::vec3 &force) {
        body->applyCentralForce(btVector3(force.x, force.y, force.z));
    }

    void Rigidbody3D::AddTorque(const glm::vec3 &torque) {
        body->applyTorque(btVector3(torque.x, torque.y, torque.z));
    }

    void Rigidbody3D::AddForceAtPosition(const glm::vec3 &force, const glm::vec3 &position) {
        body->applyForce(btVector3(force.x, force.y, force.z),
                         btVector3(position.x, position.y, position.z));
    }

    // velocity functions
    void Rigidbody3D::SetVelocity(const glm::vec3 &velocity) {
        body->setLinearVelocity(
            btVector3(velocity.x, velocity.y, velocity.z));
    }

    void Rigidbody3D::SetAngularVelocity(const glm::vec3 &velocity) {
        body->setAngularVelocity(
            btVector3(velocity.x, velocity.y, velocity.z));
    }

    glm::vec3 Rigidbody3D::GetVelocity() {
        auto velocity = body->getLinearVelocity();
        return glm::vec3(velocity.x(), velocity.y(), velocity.z());
    }

    glm::vec3 Rigidbody3D::GetAngularVelocity() {
        auto velocity = body->getAngularVelocity();
        return glm::vec3(velocity.x(), velocity.y(), velocity.z());
    }

    void Rigidbody3D::Update() {
        if (!body)
            return;

        if (!Runtime::instance->isRunning) return;

        btTransform btTrans = body->getWorldTransform();
        glm::mat4 mat = glm::mat4(1.0f);
        btTrans.getOpenGLMatrix(&mat[0][0]);

        // decompose
        glm::vec3 pos, rot, scal;
        DecomposeTransform(mat, pos, rot, scal);

        transform->position = pos;
        // if (!fixedRotation) {
        transform->rotation = rot;
        // }

        if (fixedRotation) {
            body->setAngularFactor(btVector3(0, 0, 0));
        }
    }

} // namespace Engine::Components
  // namespace Engine::Components