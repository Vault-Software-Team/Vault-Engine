#pragma once

#include "mono/metadata/object.h"
#include <string>
namespace Engine {
    class CSharpHelper {
    public:
        static std::string MonoStrToString(MonoString *str);
        static MonoString *StrToMonoString(const std::string &str);
    };
} // namespace Engine