#include "Engine/Mono/CSharp.hpp"
#include "mono/metadata/object.h"
#include "mono/utils/mono-publib.h"
#include <Engine/Mono/HelperFunctions.hpp>
#include <string>

namespace Engine {
    std::string CSharpHelper::MonoStrToString(MonoString *str) {
        char *ch = mono_string_to_utf8(str);
        const std::string cxx_str(ch);
        mono_free(ch);

        return cxx_str;
    }

    MonoString *CSharpHelper::StrToMonoString(const std::string &str) {
        return mono_string_new(CSharp::instance->app_domain, str.c_str());
    }

    std::string CSharpHelper::Vec2ToStr(const glm::vec2 &_) {
        return std::to_string(_.x) + " " + std::to_string(_.y);
    }

    std::string CSharpHelper::Vec3ToStr(const glm::vec3 &_) {
        return std::to_string(_.x) + " " + std::to_string(_.y) + " " + std::to_string(_.z);
    }

    std::string CSharpHelper::Vec4ToStr(const glm::vec4 &_) {
        return std::to_string(_.x) + " " + std::to_string(_.y) + " " + std::to_string(_.z) + " " + std::to_string(_.w);
    }
} // namespace Engine