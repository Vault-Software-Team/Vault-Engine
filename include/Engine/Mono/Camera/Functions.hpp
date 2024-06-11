#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        float Camera_GetKey(MonoString *key, MonoString *id);
        void Camera_SetKey(MonoString *key, MonoString *id, float value);
        MonoString *Camera_GetFront(MonoString *id);
        bool Camera_GetBool(MonoString *key, MonoString *id);
        void Camera_SetBool(MonoString *key, MonoString *id, bool value);
    } // namespace CSharpInternalFunctions
} // namespace Engine