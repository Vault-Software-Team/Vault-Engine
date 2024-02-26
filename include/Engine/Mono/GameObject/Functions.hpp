#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *GameObject_GetName(MonoString *ID);
        MonoString *GameObject_GetTag(MonoString *ID);
        MonoString *GameObject_GetIDByName(MonoString *name);
        MonoString *GameObject_GetIDByTag(MonoString *tag);
    } // namespace CSharpInternalFunctions
} // namespace Engine