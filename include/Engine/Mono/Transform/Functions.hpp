#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        void Transform_GetPosition(MonoString *ID, MonoString **out);
        void Transform_GetRotation(MonoString *ID, MonoString **out);
        void Transform_GetScale(MonoString *ID, MonoString **out);
        void Transform_SetField(MonoString *ID, MonoString *key, float x, float y, float z);
    } // namespace CSharpInternalFunctions
} // namespace Engine