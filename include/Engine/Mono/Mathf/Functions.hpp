#pragma once

#include "glm/fwd.hpp"
#include "mono/metadata/object.h"
#include <mono/metadata/assembly.h>
#include <mono/jit/jit.h>

namespace Engine {
    namespace CSharpInternalFunctions::Mathf {
        float Deg2Rad(float value);
        float Rad2Deg(float value);
        float Abs(float value);
        float Acos(float value);
        float Asin(float value);
        float Atan(float value);
        float Atan2(float x, float y);
        float Ceil(float value);
        float Clamp(float value, float max, float min);
        float Cos(float value);
        float Sin(float value);
        float Sqrt(float value);
        float Tan(float value);
        float Round(float value);
        float Pow(float x, float y);
        float Log(float value);
        float Log10(float value);
        float Max(float x, float y);
        float Min(float x, float y);
        float Exp(float x);
        float Lerp(float a, float b, float t);
        int RandomRange(int min, int max);
        float FloatRandomRange(float min, float max);
    } // namespace CSharpInternalFunctions::Mathf
} // namespace Engine