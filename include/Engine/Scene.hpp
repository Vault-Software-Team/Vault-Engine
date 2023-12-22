#pragma once
#include <memory>
#include "GameObject.hpp"
#include <vector>

namespace Engine {
    class Scene {
    public:
        static void New(const std::string &scene_file);

        static inline std::unique_ptr<Scene> Main; // has to be a raw pointer sadly;
        entt::registry EntityRegistry;
        std::vector<std::unique_ptr<GameObject>> GameObjects;
        std::string scene_file_path;

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine