#pragma once
#include <functional>
#include <glm/ext.hpp>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btSoftBodySolvers.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyData.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <string>
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "LinearMath/btDefaultMotionState.h"

namespace Engine {
    class Physics3D {
    public:
        static Physics3D *instance;
        btDiscreteDynamicsWorld *dynamicsWorld;
        btBroadphaseInterface *broadphase;
        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher *dispatcher;
        btSequentialImpulseConstraintSolver *solver;
        btAlignedObjectArray<btCollisionShape *> collisionShapes;
        btAlignedObjectArray<btRigidBody *> rigidBodies;
        btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
        btAlignedObjectArray<btTypedConstraint *> constraints;
        btAlignedObjectArray<btCollisionObject *> collisionObjects;
        glm::vec3 gravity = glm::vec3(0, -9.81, 0);
        static inline const glm::vec3 DefaultGravity = glm::vec3(0, -9.81, 0);

        Physics3D(const glm::vec3 &gravity = glm::vec3(0, -9.81, 0));
        ~Physics3D();

        void InitPhysics();
        void DeletePhysics();
        void SetGlobalGravity(const glm::vec3 gravity);

        void UpdatePhysics();
        void CollisionCallback(std::function<void(const std::string &, const std::string &)> HandleEntities);
    };
} // namespace Engine