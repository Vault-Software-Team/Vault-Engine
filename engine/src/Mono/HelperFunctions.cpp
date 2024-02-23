#include "Engine/Mono/CSharp.hpp"
#include "mono/metadata/object.h"
#include "mono/utils/mono-publib.h"
#include <Engine/Mono/HelperFunctions.hpp>

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
} // namespace Engine