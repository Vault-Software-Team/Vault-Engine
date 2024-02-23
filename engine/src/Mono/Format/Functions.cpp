#include "mono/metadata/object.h"
#include <Engine/Mono/Format/Functions.hpp>
#include <string>
#include <Engine/Mono/HelperFunctions.hpp>

namespace Engine::CSharpInternalFunctions {
    MonoString *float_ToString(float f) {
        return CSharpHelper::StrToMonoString(std::to_string(f));
    }
    MonoString *double_ToString(double f) {
        return CSharpHelper::StrToMonoString(std::to_string(f));
    }
    MonoString *int_ToString(int f) {
        return CSharpHelper::StrToMonoString(std::to_string(f));
    }
} // namespace Engine::CSharpInternalFunctions