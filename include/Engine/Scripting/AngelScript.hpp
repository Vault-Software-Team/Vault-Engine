#pragma once
#include <dllapi.hpp>
#include <memory>
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

namespace Engine::Scripting::AngelScript {

    class DLL_API AngelScriptEngine {
    public:
        static DLL_API AngelScriptEngine *Main;
        asIScriptEngine *m_AsEngine;

        AngelScriptEngine();
        ~AngelScriptEngine();

        int RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *auxiliary = 0);
        void LoadScript(const std::string &module_name, const std::string &script_path);
        asIScriptModule *GetModule(const std::string &module_name);
        asIScriptFunction *GetFunctionByDeclaration(asIScriptModule *module, const std::string &declaration);
        asIScriptContext *CreateContext();
        int PrepareAndExecuteFunction(asIScriptContext *context, asIScriptFunction *function);
    };
} // namespace Engine::Scripting::AngelScript