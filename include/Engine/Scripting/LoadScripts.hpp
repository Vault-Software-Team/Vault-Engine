#pragma once
#include <vector>
#include <memory>
#include <dllapi.hpp>
#include <Engine/Scripting/Script.hpp>

#define VAULTAPI_CREATE_FUNCTION_NAME "VAULTAPI_CREATE_SCRIPT_INSTANCE"

namespace Engine::Scripting::CXX {

    class DLL_API CXXScripting {
    public:
        struct DLL_API SharedObject {
            std::string name;
            std::string typeName;
            void *handle;
            Script *(*create_script)();
        };

        static DLL_API std::vector<SharedObject> dl_script_handles;

        static void LoadScripts();
        static void CompileScripts();

        static SharedObject &GetSharedObject(const std::string &name);
    };
} // namespace Engine::Scripting::CXX