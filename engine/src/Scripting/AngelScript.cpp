#include <Engine/Scripting/AngelScript.hpp>
#include <cassert>

// NOTE: This is experimental! Might go down with C# hell still

namespace Engine::Scripting::AngelScript {
    DLL_API AngelScriptEngine *AngelScriptEngine::Main;
    void MessageCallback(const asSMessageInfo *msg, void *param) {
        const char *type = "ERR ";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";
        printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
    }

    AngelScriptEngine::AngelScriptEngine() {
        m_AsEngine = asCreateScriptEngine();

        int r = m_AsEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        assert(r >= 0);

        RegisterStdString(m_AsEngine);
        Main = this;
    }

    AngelScriptEngine::~AngelScriptEngine() {
    }

    int AngelScriptEngine::RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *auxiliary) {
        return m_AsEngine->RegisterGlobalFunction(declaration, funcPointer, callConv, auxiliary);
    }

    void AngelScriptEngine::LoadScript(const std::string &module_name, const std::string &script_path) {
        // copied from doc cuz im lazy
        CScriptBuilder builder;
        int r = builder.StartNewModule(m_AsEngine, module_name.c_str());
        if (r < 0) {
            // If the code fails here it is usually because there
            // is no more memory to allocate the module
            printf("Unrecoverable error while starting a new module.\n");
            return;
        }
        r = builder.AddSectionFromFile(script_path.c_str());
        if (r < 0) {
            printf("The builder wasn't able to load the file.\n");
            return;
        }
        r = builder.BuildModule();
        if (r < 0) {
            // An error occurred. Instruct the script writer to fix the
            // compilation errors that were listed in the output stream.
            printf("Please correct the errors in the script and try again.\n");
            return;
        }
    }

    asIScriptModule *AngelScriptEngine::GetModule(const std::string &module_name) {
        return m_AsEngine->GetModule(module_name.c_str());
    }

    asIScriptFunction *AngelScriptEngine::GetFunctionByDeclaration(asIScriptModule *module, const std::string &declaration) {
        return module->GetFunctionByDecl(declaration.c_str());
    }

    asIScriptContext *AngelScriptEngine::CreateContext() {
        return m_AsEngine->CreateContext();
    }

    int AngelScriptEngine::PrepareAndExecuteFunction(asIScriptContext *context, asIScriptFunction *function) {
        context->Prepare(function);
        int r = context->Execute();
        if (r != asEXECUTION_FINISHED) {
            // The execution didn't complete as expected. Determine what happened.
            if (r == asEXECUTION_EXCEPTION) {
                // An exception occurred, let the script writer know what happened so it can be corrected.
                printf("An exception '%s' occurred. Please correct the code and try again.\n", context->GetExceptionString());
            }
        }
        return r;
    }

    // TODO: https://www.angelcode.com/angelscript/sdk/docs/manual/doc_use_script_class.html

} // namespace Engine::Scripting::AngelScript