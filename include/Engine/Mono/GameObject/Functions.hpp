#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *GameObject_GetName(MonoString *ID);
        MonoString *GameObject_GetTag(MonoString *ID);
    } // namespace CSharpInternalFunctions
} // namespace Engine