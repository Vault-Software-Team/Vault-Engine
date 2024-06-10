#pragma once

#include "glm/fwd.hpp"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine::CSharpInternalFunctions {
        MonoObject *Entity_GetClassInstance(MonoString *ID, MonoString *type);
} // namespace Engine::CSharpInternalFunctions
