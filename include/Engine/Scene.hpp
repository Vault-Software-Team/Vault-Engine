#pragma once
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include <dllapi.hpp>

namespace Engine {
    class GameObject;

    class DLL_API Scene {
    public:
        static void New(const std::string &scene_file);

        static inline std::unique_ptr<Scene> Main; // has to be a raw pointer sadly;
        entt::registry EntityRegistry;
        std::vector<std::shared_ptr<GameObject>> GameObjects;
        std::string scene_file_path;

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine