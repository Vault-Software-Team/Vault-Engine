#pragma once

#include "glm/fwd.hpp"
#include "mono/metadata/object.h"
#include <string>
#include <glm/ext.hpp>
namespace Engine {
    class CSharpHelper {
    public:
        static std::string MonoStrToString(MonoString *str);
        static MonoString *StrToMonoString(const std::string &str);
        static std::string Vec2ToStr(const glm::vec2 &_);
        static std::string Vec3ToStr(const glm::vec3 &_);
        static std::string Vec4ToStr(const glm::vec4 &_);
    };
} // namespace Engine