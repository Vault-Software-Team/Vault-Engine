#pragma once
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include <dllapi.hpp>
#include <Engine/Components/Camera.hpp>

namespace Engine {
    class GameObject;

    class DLL_API Scene {
    public:
        static void New(const std::string &scene_file);

        static inline std::unique_ptr<Scene> Main;
        Components::Camera *main_camera_object = nullptr;
        entt::registry EntityRegistry;
        std::vector<std::shared_ptr<GameObject>> GameObjects;
        std::string scene_file_path;

        void SetMainCameraObject(const std::shared_ptr<GameObject> &camObject);

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine