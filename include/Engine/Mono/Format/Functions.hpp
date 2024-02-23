#pragma once

#include "glm/fwd.hpp"
#include "mono/metadata/object.h"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions {
        MonoString *float_ToString(float f);
        MonoString *double_ToString(double f);
        MonoString *int_ToString(int f);
    } // namespace CSharpInternalFunctions
} // namespace Engine