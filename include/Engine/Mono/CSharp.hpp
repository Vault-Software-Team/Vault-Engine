#pragma once

#include "mono/metadata/image.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

namespace Engine {
    class CSharp {
    public:
        static CSharp *instance;

        MonoDomain *root_domain;
        MonoDomain *app_domain;
        MonoAssembly *core_assembly;

        CSharp(const std::string &lib_path, const std::string &runtime_name = "VaultScriptRuntime", const std::string &appdomain_name = "VaultAppDomain");
        ~CSharp();
    };
} // namespace Engine