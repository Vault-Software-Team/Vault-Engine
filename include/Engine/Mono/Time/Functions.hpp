#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        float Time_GetDeltaTime();
    } // namespace CSharpInternalFunctions
} // namespace Engine