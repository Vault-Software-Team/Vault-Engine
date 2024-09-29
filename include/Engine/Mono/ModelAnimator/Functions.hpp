#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *ModelAnimator_GetCurrentAnimation(MonoString *id);
        void ModelAnimator_SetCurrentAnimation(MonoString *id, MonoString *value);
    } // namespace CSharpInternalFunctions
} // namespace Engine