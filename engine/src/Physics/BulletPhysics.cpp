#include "Engine/Runtime.hpp"
#include <Engine/Physics/BulletPhysics.hpp>

namespace Engine {
    Physics3D *Physics3D::instance = nullptr;
    Physics3D::Physics3D(const glm::vec3 &gravity) : gravity(gravity) {
        if (instance) {
            std::cout << "Only one instance of Physics3D can exist! you fucking moron\n";
        }
        instance = this;

        InitPhysics();
    }

    Physics3D::~Physics3D() {
    }
    void Physics3D::InitPhysics() {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    }

    void Physics3D::DeletePhysics() {
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }

    void Physics3D::SetGlobalGravity(const glm::vec3 gravity) {
        this->gravity = gravity;
    }

    void Physics3D::UpdatePhysics() {
        dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
        dynamicsWorld->stepSimulation(Runtime::instance->timestep, 10);
    }

    void Physics3D::CollisionCallback(std::function<void(const std::string &, const std::string &)> HandleEntities) {
        dynamicsWorld->performDiscreteCollisionDetection();
        int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();

        // do collision callback on all rigid bodies
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            const auto *obA = static_cast<const btCollisionObject *>(contactManifold->getBody0());
            const auto *obB = static_cast<const btCollisionObject *>(contactManifold->getBody1());

            // get the entity names
            auto *entityA = static_cast<std::string *>(obA->getUserPointer());
            auto *entityB = static_cast<std::string *>(obB->getUserPointer());

            // call the callback
            HandleEntities(*entityA, *entityB);
        }
    }
} // namespace Engine