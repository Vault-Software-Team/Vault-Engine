#pragma once
#include <dllapi.hpp>
#include <Engine/GameObject.hpp>
#include <string>
#include <vector>

namespace Editor {
    char asciitolower(char in);
    struct DLL_API Log {
        enum LogType {
            LOG_INFO,
            LOG_WARNING,
            LOG_ERROR,
        } type;
        std::string content;
    };

    class DLL_API GUI {
    private:
        static DLL_API DLL_API std::vector<Log> logs;

    public:
        static DLL_API Engine::GameObject *selected_gameObject;
        static DLL_API uint32_t framebufferTextureID;
        static DLL_API void LogInfo(const std::string &content);
        static DLL_API void LogError(const std::string &content);
        static DLL_API void LogWarning(const std::string &content);

        static DLL_API void SetNameIcon(std::string &txt, Engine::GameObject *gameObject);
        static DLL_API void Hierarchy();
        static DLL_API void Assets();
        static DLL_API void Console();
        static DLL_API void Scene();
        static DLL_API void Components();
    };
} // namespace Editor