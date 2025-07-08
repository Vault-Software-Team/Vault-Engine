#pragma once
#include <spdlog/spdlog.h>
#include <dllapi.hpp>
#include <memory>

namespace VaultRenderer {
    class Logger {
    public:
        static Logger *instance;
        std::shared_ptr<spdlog::logger> engine_logger;
        std::shared_ptr<spdlog::logger> app_logger;

        Logger();
    };
} // namespace VaultRenderer

#define ENGINE_ERROR(...) spdlog::get("VAULT")->error(__VA_ARGS__)
#define ENGINE_WARN(...) spdlog::get("VAULT")->warn(__VA_ARGS__)
#define ENGINE_INFO(...) spdlog::get("VAULT")->info(__VA_ARGS__)
#define ENGINE_TRACE(...) spdlog::get("VAULT")->trace(__VA_ARGS__)
#define ENGINE_CRITICAL(...) spdlog::get("VAULT")->critical(__VA_ARGS__)
#define ENGINE_DEBUG(...) spdlog::get("VAULT")->debug(__VA_ARGS__)
#define APP_ERROR(...) spdlog::get("APP")->error(__VA_ARGS__)
#define APP_WARN(...) spdlog::get("APP")->warn(__VA_ARGS__)
#define APP_INFO(...) spdlog::get("APP")->info(__VA_ARGS__)
#define APP_TRACE(...) spdlog::get("APP")->trace(__VA_ARGS__)
#define APP_CRITICAL(...) spdlog::get("APP")->critical(__VA_ARGS__)
#define APP_DEBUG(...) spdlog::get("APP")->debug(__VA_ARGS__)

// thanks to my friend void for this, i was gonna write my own but i got lazy so i just copied his
#ifndef VAULT_RELEASE_MODE
    #define ENGINE_ASSERT(condition, message)                                                                             \
        do {                                                                                                              \
            if (!(condition)) {                                                                                           \
                ENGINE_CRITICAL("Assertion \"" #condition "\" failed in {0} line {1}: {2}", __FILE__, __LINE__, message); \
                std::terminate();                                                                                         \
            }                                                                                                             \
        } while (false)

    #define APP_ASSERT(condition, message)                                                                             \
        do {                                                                                                           \
            if (!(condition)) {                                                                                        \
                APP_CRITICAL("Assertion \"" #condition "\" failed in {0} line {1}: {2}", __FILE__, __LINE__, message); \
                std::terminate();                                                                                      \
            }                                                                                                          \
        } while (false)

#else
    #define ENGINE_ASSERT(condition, message) \
        do {                                  \
        } while (false)

    #define APP_ASSERT(condition, message) \
        do {                               \
        } while (false)
#endif