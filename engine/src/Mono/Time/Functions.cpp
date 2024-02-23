#include <Engine/Mono/Time/Functions.hpp>
#include "Engine/Runtime.hpp"

namespace Engine::CSharpInternalFunctions {
    float Time_GetDeltaTime() {
        return Runtime::instance->timestep;
    }
} // namespace Engine::CSharpInternalFunctions