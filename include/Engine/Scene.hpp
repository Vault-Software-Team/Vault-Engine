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
        static inline Components::Camera *EditorSceneCamera = nullptr;
        Components::Camera *main_camera_object = nullptr;
        entt::registry EntityRegistry;
        std::vector<std::shared_ptr<GameObject>> GameObjects;
        static inline std::vector<std::shared_ptr<GameObject>> StaticGameObjects;
        std::string scene_file_path;

        void SetMainCameraObject(const std::shared_ptr<GameObject> &camObject);
        static void MakeSceneCamera();
        void UpdateGameObjectComponents();
        static void UpdateStaticGameObjectComponents();

        void DeleteStaticGameObject(std::shared_ptr<GameObject> &gameObject);
        void SetSceneCameraAsMain();

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine