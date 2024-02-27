#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        void Rigidbody2D_GetKey(MonoString *key, MonoString *id, MonoString **result);
        void Rigidbody2D_SetVelocity(float x, float y, MonoString *id);
        void Rigidbody2D_SetAngularVelocity(float velocity, MonoString *id);
        void Rigidbody2D_SetPosition(float x, float y, MonoString *id);
        void Rigidbody2D_Force(float x, float y, MonoString *id);
        void Rigidbody2D_Torque(float torque, MonoString *id);
        void Rigidbody2D_SetType(int type, MonoString *id);
    } // namespace CSharpInternalFunctions
} // namespace Engine