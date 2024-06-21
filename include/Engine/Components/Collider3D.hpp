#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "Engine/Components/MeshRenderer.hpp"
#include "LinearMath/btVector3.h"
#include "Transform.hpp"
#include <Engine/Physics/BulletPhysics.hpp>

namespace Engine {
    namespace Components {
        struct DLL_API Collider3D : public Base {
            btCollisionShape *shape = nullptr;
            virtual void CreateShape() {}

            Transform *transform;

            // make Collider3D be shape so for example you can do
            // collider3D->setMargin(0.05f);
            Collider3D() = default;
            Collider3D(const Collider3D &) = default;
            explicit operator const btCollisionShape *() const { return shape; }

            void Init() override;
        };

        struct DLL_API BoxCollider3D : Collider3D {
            static inline const std::string display_name = "Box Collider 3D";

            glm::vec3 size = glm::vec3(1, 1, 1);

            void CreateShape() override {
                shape = new btBoxShape(btVector3((size.x / 2) / 2, (size.y / 2) / 2, (size.z / 2) / 2));
                shape->setMargin(0.05f);
            }

            void OnGUI() override;
        };

        struct DLL_API MeshCollider3D : Collider3D {
            static inline const std::string display_name = "Mesh Collider 3D";
            glm::vec3 size = glm::vec3(1, 1, 1);

            void CreateShape(MeshRenderer *renderer) {
                if (renderer->mesh) {
                    auto mesh = renderer->mesh;

                    shape = new btConvexHullShape();
                    for (auto &vertex : mesh->vertices) {
                        ((btConvexHullShape *)shape)
                            ->addPoint(btVector3(vertex.position.x,
                                                 vertex.position.y,
                                                 vertex.position.z));
                    }
                    // add transform scale to shape
                    shape->setLocalScaling(
                        btVector3(size.x / 2, size.y / 2, size.z / 2));
                    shape->setMargin(0.05f);
                }
            }

            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine