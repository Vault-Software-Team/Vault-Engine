#pragma once

#include "mono/metadata/image.h"
#include "mono/metadata/object-forward.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>

namespace Engine {
    class CSharp {
    public:
        const std::string runtime_name, appdomain_name;
        static CSharp *instance;

        MonoDomain *root_domain;
        MonoDomain *app_domain;
        MonoAssembly *core_assembly;
        MonoImage *core_assembly_image;

        CSharp(const std::string &lib_path, const std::string &runtime_name = "VaultScriptRuntime", const std::string &appdomain_name = "VaultAppDomain");
        ~CSharp();

        void InitRuntime();
        void InitMono();
        void ReloadAssembly();
        static MonoImage *GetImage(MonoAssembly *core_assembly);
    };

    class CSharpClass {
    public:
        MonoClass *klass;
        MonoObject *instance;
        uint32_t gc_handle;

        CSharpClass(MonoImage *image, const std::string &name_space, const std::string &name);

        MonoMethod *GetMethod(const std::string &name, int param_count);
        void *GetThunkFromMethod(MonoMethod *method);
        void *GetMethodThunk(const std::string &name, int param_count);
        MonoObject *GetHandleTarget();
    };
} // namespace Engine