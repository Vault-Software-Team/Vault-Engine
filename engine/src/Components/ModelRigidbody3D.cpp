#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "Engine/Components/ModelRenderer.hpp"
#include "Engine/DecomposeTransform.hpp"
#include "Engine/Physics/BulletPhysics.hpp"
#include <Engine/Components/ModelRigidbody3D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void ModelRigidbody3D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
        renderer = &Scene::Main->EntityRegistry.get<ModelRenderer>(entity);
    }

    void ModelRigidbody3D::OnGUI() {
        DrawComponent<ModelRigidbody3D>(Scene::Main->EntityRegistry, 40, [&] {
            ImGui::DragFloat("Mass", &mass, 0.01f);
            ImGui::DragFloat("Friction", &friction, 0.01f);
            ImGui::DragFloat("Restitution", &restitution, 0.01f);
            ImGui::Checkbox("Trigger", &trigger);
            ImGui::Checkbox("Fixed Rotation", &fixedRotation);
        });
    }

    void ModelRigidbody3D::CreateBodiesFromMeshes() {
        for (auto &mesh : renderer->model->meshes) {
            rigidbodies[&mesh] = std::make_unique<MRBody>();
            auto &body = rigidbodies[&mesh];

            body->ref = std::make_shared<btConvexHullShape>();
            body->meshRef = &mesh;

            for (auto &vertex : mesh.vertices) {
                ((btConvexHullShape *)body->ref.get())
                    ->addPoint(btVector3(vertex.position.x,
                                         vertex.position.y,
                                         vertex.position.z));
            }
            // add transform scale to shape
            body->ref->setLocalScaling(btVector3(body->localScale.x / 2, body->localScale.y / 2, body->localScale.z / 2));
            body->ref->setMargin(0.05f);

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
                body->ref->calculateLocalInertia(mass, inertia);
            }

            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, body->ref.get(), inertia);
            rbInfo.m_restitution = restitution;
            rbInfo.m_friction = friction;
            body->body = std::make_shared<btRigidBody>(rbInfo);
            // set user data
            body->body->setUserPointer(&ID);
            // set trigger
            body->body->setCollisionFlags(body->body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
            if (trigger) {
                body->body->setCollisionFlags(
                    body->body->getCollisionFlags() |
                    btCollisionObject::CO_GHOST_OBJECT);

                body->body->setCollisionFlags(body->body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

                // stupid ass conversion
                body->body = std::shared_ptr<btRigidBody>((btRigidBody *)new btGhostObject());
            }

            if (fixedRotation) {
                body->body->setAngularFactor(btVector3(0, 0, 0));
            }

            Physics3D::instance->dynamicsWorld->addRigidBody(body->body.get(), 1, 1);
        }
    }

    void ModelRigidbody3D::AddForce(const glm::vec3 &force) {
        body->applyCentralForce(btVector3(force.x, force.y, force.z));
    }

    void ModelRigidbody3D::AddTorque(const glm::vec3 &torque) {
        body->applyTorque(btVector3(torque.x, torque.y, torque.z));
    }

    void ModelRigidbody3D::AddForceAtPosition(const glm::vec3 &force, const glm::vec3 &position) {
        body->applyForce(btVector3(force.x, force.y, force.z),
                         btVector3(position.x, position.y, position.z));
    }

    // velocity functions
    void ModelRigidbody3D::SetVelocity(const glm::vec3 &velocity) {
        body->setLinearVelocity(
            btVector3(velocity.x, velocity.y, velocity.z));
    }

    void ModelRigidbody3D::SetAngularVelocity(const glm::vec3 &velocity) {
        body->setAngularVelocity(
            btVector3(velocity.x, velocity.y, velocity.z));
    }

    glm::vec3 ModelRigidbody3D::GetVelocity() {
        auto velocity = body->getLinearVelocity();
        return glm::vec3(velocity.x(), velocity.y(), velocity.z());
    }

    glm::vec3 ModelRigidbody3D::GetAngularVelocity() {
        auto velocity = body->getAngularVelocity();
        return glm::vec3(velocity.x(), velocity.y(), velocity.z());
    }

    void ModelRigidbody3D::Update() {
        if (!body)
            return;

        for (auto &_ : rigidbodies) {
            auto &rb = _.second;

            btTransform btTrans = body->getWorldTransform();
            glm::mat4 mat = glm::mat4(1.0f);
            btTrans.getOpenGLMatrix(&mat[0][0]);

            // decompose
            glm::vec3 position, rotation, scale;
            DecomposeTransform(mat, position, rotation, scale);

            rb->meshRef->transform = glm::translate(glm::mat4(1.0f), position) *
                                     glm::toMat4(glm::quat(fixedRotation ? rotation : glm::vec3(0, 0, 0))) *
                                     glm::scale(glm::mat4(1.0f),
                                                glm::vec3(scale.x * 0.5, scale.y * 0.5,
                                                          scale.z * 0.5));

            if (fixedRotation) {
                body->setAngularFactor(btVector3(0, 0, 0));
            }
        }
    }

} // namespace Engine::Components
  // namespace Engine::Components