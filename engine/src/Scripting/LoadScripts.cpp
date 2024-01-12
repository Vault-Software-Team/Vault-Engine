#include <Engine/Scripting/LoadScripts.hpp>
#include <filesystem>
#include <regex>
#include <iostream>

#ifndef _WIN32
#include <dlfcn.h>
#endif
namespace fs = std::filesystem;

namespace Engine::Scripting::CXX {
    DLL_API std::vector<CXXScripting::SharedObject> CXXScripting::dl_script_handles;

    void CXXScripting::CompileScripts() {
        static const std::string include_flags = "-I../include -I../vendor";
        static const std::string linuxCompiler = "g++";

        fs::recursive_directory_iterator iter("../assets");
        for (auto &dirEntry : iter) {
            if (!dirEntry.path().string().ends_with(".cpp"))
                continue;
            std::string file = dirEntry.path().string();
            file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

            std::string objFile = dirEntry.path().string();
            objFile = std::regex_replace(objFile, std::regex("\\.cpp"), ".o");
            std::string CompileToObject_Command = (linuxCompiler + " -c -g -fPIC " + dirEntry.path().string() + " " + include_flags + " -rdynamic -o " + objFile).c_str();
            std::string CompileToSharedObject_Command = (linuxCompiler + " -g -shared " + objFile + " -o " + file);
            /*
            CompileToObject_Command:
                g++ -c -fPIC script_file.cpp -Iinclude_directory -rdynamic -o script_file.o

            CompileToObject_Shared:
                g++ -shared script_file.o -o script_file.so

            Basically these are the commands that get run
            */

            system(CompileToObject_Command.c_str());
            system(CompileToSharedObject_Command.c_str());

            std::cout << "VAULT_API --> Successfully compiled " << dirEntry.path().string() << " to a shared object!\n";
        }
        std::cout << "VAULT_API --> Loading Scripts...\n";
        for (auto script : dl_script_handles) {
            if (script.handle)
                dlclose(script.handle);
        }
        dl_script_handles.clear();
        LoadScripts();
        std::cout << "VAULT_API --> Successfully Loaded Scripts!\n";
    }

    void CXXScripting::LoadScripts() {
        fs::recursive_directory_iterator iter("../assets");

        for (auto &dirEntry : iter) {
            if (dirEntry.path().string().ends_with(".cpp")) {
                std::string file = dirEntry.path().string();
                file = std::regex_replace(file, std::regex("\\.cpp"), ".so");

                SharedObject obj;

                std::cout << file << "\n";
                obj.handle = dlopen(fs::absolute(dirEntry.path()).string().c_str(), RTLD_LAZY);
                std::cout << obj.handle << "\n";
                obj.create_script = (Script * (*)()) dlsym(obj.handle, "create_script");
                std::cout << obj.create_script << "\n";

                obj.name = dirEntry.path().filename().string();
                std::cout << obj.name << "\n";
                dl_script_handles.push_back(obj);
            }
        }
    }

    CXXScripting::SharedObject &CXXScripting::GetSharedObject(const std::string &name) {
        for (auto &obj : dl_script_handles) {
            if (obj.name == name)
                ;
            return obj;
        }
    }
} // namespace Engine::Scripting::CXX