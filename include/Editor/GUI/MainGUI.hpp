#pragma once
#include <Engine/GameObject.hpp>
#include <string>
#include <dllapi.hpp>
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
        static inline std::vector<Log> logs;

    public:
        static inline uint32_t framebufferTextureID;
        static void LogInfo(const std::string &content);
        static void LogError(const std::string &content);
        static void LogWarning(const std::string &content);

        static void SetNameIcon(std::string &txt, Engine::GameObject *gameObject);
        static void Hierarchy();
        static void Assets();
        static void Console();
        static void Scene();
    };
} // namespace Editor