#include "mono/metadata/assembly.h"
#include "mono/metadata/loader.h"
#include "mono/metadata/object-forward.h"
#include "mono/metadata/object.h"
#include <Engine/Mono/CSharp.hpp>
#include <iostream>
#include <mono/jit/jit.h>
#include <mono/metadata/appdomain.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace Engine {
    CSharp *CSharp::instance;

    char *ReadBytes(const std::string &filepath, uint32_t *outSize) {
        std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

        if (!stream) {
            // Failed to open the file
            return nullptr;
        }

        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        uint32_t size = end - stream.tellg();

        if (size == 0) {
            // File is empty
            return nullptr;
        }

        char *buffer = new char[size];
        stream.read((char *)buffer, size);
        stream.close();

        *outSize = size;
        return buffer;
    }

    MonoAssembly *LoadCSharpAssembly(const std::string &assemblyPath) {
        uint32_t fileSize = 0;
        char *fileData = ReadBytes(assemblyPath, &fileSize);

        // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        MonoImage *image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

        if (status != MONO_IMAGE_OK) {
            const char *errorMessage = mono_image_strerror(status);
            // Log some error message using the errorMessage data
            return nullptr;
        }

        MonoAssembly *assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
        mono_image_close(image);

        // Don't forget to free the file data
        delete[] fileData;

        return assembly;
    }

    void PrintAssemblyTypes(MonoAssembly *assembly) {
        MonoImage *image = mono_assembly_get_image(assembly);
        const MonoTableInfo *typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; i++) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char *nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

            printf("%s.%s\n", nameSpace, name);
        }
    }

    MonoClass *GetClassInAssembly(MonoAssembly *assembly, const char *namespaceName, const char *className) {
        MonoImage *image = mono_assembly_get_image(assembly);
        MonoClass *klass = mono_class_from_name(image, namespaceName, className);

        if (klass == nullptr) {
            // Log error here
            return nullptr;
        }

        return klass;
    }

    void CSharp::InitRuntime() {
        if (root_domain != nullptr) return;
        root_domain = mono_jit_init((char *)runtime_name.c_str());
    }

    void CSharp::InitMono() {
        mono_set_assemblies_path("../mono/lib");
        InitRuntime();

        app_domain = mono_domain_create_appdomain((char *)appdomain_name.c_str(), nullptr);
        mono_domain_set(app_domain, true);

        core_assembly = LoadCSharpAssembly("../csharp-lib/bin/Debug/net8.0/csharp-lib.dll");
        PrintAssemblyTypes(core_assembly);

        core_assembly_image = GetImage(core_assembly);
        // CSharpClass klass(core_assembly_image, "", "MyScript");
        ScriptClass klass(core_assembly_image, "", "MyScript");
        klass.OnStart("someid-1234");
        klass.OnUpdate();

        // // MonoObject *__exception = nullptr;
        // // void *params = mono_string_new(app_domain, "someid");
        // // mono_runtime_invoke(klass.GetMethod("Init", 1), klass.GetHandleTarget(), &params, &__exception);
        // // auto SetObjectID_Thunk = (SetObjectIDType)klass.GetMethodThunk("SetObjectID", 1);
        // // SetObjectID_Thunk(klass.GetHandleTarget(), &params, &__exception);

        // // auto OnStart_Thunk = (OnStartType)klass.GetMethodThunk("OnStart", 0);
        // // MonoObject *exception = nullptr;
        // // OnStart_Thunk(klass.GetHandleTarget(), &exception);

        // // if (exception) {
        // //     MonoObject *exc = NULL;
        // //     MonoString *str = mono_object_to_string(exception, &exc);
        // //     if (exc) {
        // //         mono_print_unhandled_exception(exc);
        // //     } else {
        // //         std::cout << mono_string_to_utf8(str) << "\n";
        // //         // Log log(mono_string_to_utf8(str), LOG_ERROR);
        // //     }
        // // }
    }

    void CSharp::ReloadAssembly() {
        // if (!fs::exists("assets/VAULT_OUT/cs-assembly.dll"))
        //     return;

        mono_domain_set(mono_get_root_domain(), false);
        mono_domain_unload(app_domain);

        InitMono();
    }

    void func() {
    }

    void CSharp::RegisterVaultFunctions() {
        RegisterFunction("Raah", (void *(*))func);
    }

    void CSharp::RegisterFunction(const std::string &cs_path, void *func) {
        mono_add_internal_call(cs_path.c_str(), func);
    }

    CSharp::CSharp(const std::string &lib_path, const std::string &runtime_name, const std::string &appdomain_name) : runtime_name(runtime_name), appdomain_name(appdomain_name) {
        instance = this;
        InitMono();
    }

    CSharp::~CSharp() {
    }

    MonoImage *CSharp::GetImage(MonoAssembly *core_assembly) {
        return mono_assembly_get_image(core_assembly);
    }

    CSharpClass::CSharpClass(MonoImage *image, const std::string &name_space, const std::string &name) {
        klass = mono_class_from_name(image, name_space.c_str(), name.c_str());
        instance = mono_object_new(CSharp::instance->app_domain, klass);
        gc_handle = mono_gchandle_new(instance, false);
    }

    MonoMethod *CSharpClass::GetMethod(const std::string &name, int param_count) {
        return mono_class_get_method_from_name(klass, name.c_str(), param_count);
    }

    void *CSharpClass::GetThunkFromMethod(MonoMethod *method) {
        return mono_method_get_unmanaged_thunk(method);
    }

    void *CSharpClass::GetMethodThunk(const std::string &name, int param_count) {
        return mono_method_get_unmanaged_thunk(mono_class_get_method_from_name(klass, name.c_str(), param_count));
    }

    MonoObject *CSharpClass::GetHandleTarget() {
        return mono_gchandle_get_target(gc_handle);
    }

    ScriptClass::ScriptClass(MonoImage *image, const std::string &name_space, const std::string &name) : CSharpClass(image, name_space, name) {
        update_method = GetMethod("OnUpdate", 0);
        update_thunk = (OnUpdateType)GetMethodThunk("OnUpdate", 0);

        start_method = GetMethod("OnStart", 1);
        start_thunk = (OnStartType)GetThunkFromMethod(start_method);
    }

    void ScriptClass::OnStart(const std::string &gameObject_ID) {
        MonoObject *exception = nullptr;
        void *p = mono_string_new(CSharp::instance->app_domain, gameObject_ID.c_str());
        mono_runtime_invoke(start_method, GetHandleTarget(), &p, &exception);

        if (exception) {
            MonoObject *exc = NULL;
            MonoString *str = mono_object_to_string(exception, &exc);
            if (exc) {
                mono_print_unhandled_exception(exc);
            } else {
                std::cout << mono_string_to_utf8(str) << "\n";
                // Log log(mono_string_to_utf8(str), LOG_ERROR);
            }
        }
    }

    void ScriptClass::OnUpdate() {
        MonoObject *exception = nullptr;
        mono_runtime_invoke(update_method, GetHandleTarget(), nullptr, &exception);

        if (exception) {
            MonoObject *exc = NULL;
            MonoString *str = mono_object_to_string(exception, &exc);
            if (exc) {
                mono_print_unhandled_exception(exc);
            } else {
                std::cout << mono_string_to_utf8(str) << "\n";
                // Log log(mono_string_to_utf8(str), LOG_ERROR);
            }
        }
    }
} // namespace Engine