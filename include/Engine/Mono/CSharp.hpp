#pragma once

#include "mono/metadata/image.h"
#include "mono/metadata/object-forward.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <string>
#include <dllapi.hpp>
#include <memory>
#include <unordered_map>
#include <utility>

typedef void (*OnStartType)(MonoObject *, MonoObject **);
typedef void (*OnUpdateType)(MonoObject *, MonoObject **);

#define VAULT_REGISTER_FUNCTION(Func) mono_add_internal_call("Vault.InternalCalls::" #Func, (void *(*))Func)
#define VAULT_REGISTER_FUNCTION_NAME(Name, Func) mono_add_internal_call(Name, (void *(*))Func)
#define VAULT_REGISTER_FUNCTION_PREFIX(Prefix, Func) mono_add_internal_call(Prefix #Func, (void *(*))Func)

namespace Engine {
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

    class ScriptClass : public CSharpClass {
    public:
        OnUpdateType update_thunk;
        OnStartType start_thunk;
        MonoMethod *update_method;
        MonoMethod *start_method;
        MonoMethod *init_method;

        MonoMethod *OnMouseEnter_method;
        MonoMethod *OnMouseExit_method;

        bool inited = false;

        ScriptClass(MonoImage *image, const std::string &name_space, const std::string &name);

        void OnStart(const std::string &gameObject_ID);
        void InitInstance(const std::string &gameObject_ID);
        void OnUpdate();

        void OnMouseEnter();
        void OnMouseExit();
    };

    class CSharp {
    public:
        const std::string runtime_name, appdomain_name;
        static CSharp *instance;

        DLL_API MonoDomain *root_domain = nullptr;
        DLL_API MonoDomain *app_domain = nullptr;
        DLL_API MonoAssembly *core_assembly = nullptr;
        DLL_API MonoImage *core_assembly_image = nullptr;
        DLL_API std::unordered_map<std::string, std::pair<std::string, std::string>> entity_classes;
        DLL_API std::unordered_map<std::string, std::unique_ptr<CSharpClass>> command_classes;

        CSharp(const std::string &lib_path, const std::string &runtime_name = "VaultScriptRuntime", const std::string &appdomain_name = "VaultAppDomain");
        ~CSharp();

        void InitRuntime();
        void InitMono();
        void ReloadAssembly();
        void RegisterFunction(const std::string &cs_path, void *func);
        void RegisterVaultFunctions();
        void CompileAssemblies();
        void DevConsoleSetup(MonoAssembly *core_assembly);
        static MonoImage *GetImage(MonoAssembly *core_assembly);
    };
} // namespace Engine