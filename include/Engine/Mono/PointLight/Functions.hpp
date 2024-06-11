#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *PointLight_GetKey(MonoString *key, MonoString *id);
        void PointLight_SetKey(MonoString *key, MonoString *id, MonoString *value);
    } // namespace CSharpInternalFunctions
} // namespace Engine