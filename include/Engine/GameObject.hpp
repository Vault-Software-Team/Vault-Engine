#pragma once
#include <string>
#include <entt/entt.hpp>

namespace Engine {
    class GameObject {
    public:
        static inline std::unique_ptr<GameObject> New(const std::string &name, const std::string &tag = "Default");

        std::string ID;
        entt::entity entity;
        std::list<std::unique_ptr<GameObject>> children;

        GameObject(const std::string &name, const std::string &tag = "Default");
    };
} // namespace Engine