#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        void Rigidbody3D_GetKey(MonoString *key, MonoString *id, MonoString **result);
        void Rigidbody3D_SetKey(MonoString *key, MonoString *id, MonoString *value);

        void Rigidbody3D_AddForce(MonoString *id, float x, float y, float z);
        void Rigidbody3D_AddTorque(MonoString *id, float x, float y, float z);
        void Rigidbody3D_AddForceAtPosition(MonoString *id, float fx, float fy, float fz, float px, float py, float pz);
        void Rigidbody3D_SetVelocity(MonoString *id, float x, float y, float z);
        void Rigidbody3D_SetAngularVelocity(MonoString *id, float x, float y, float z);
    } // namespace CSharpInternalFunctions
} // namespace Engine