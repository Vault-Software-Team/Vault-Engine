#pragma once
#include <dllapi.hpp>
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include <Engine/Components/Camera.hpp>

namespace Engine {
    class GameObject;

    class DLL_API Scene {
    public:
        static DLL_API void New(const std::string &scene_file);

        static DLL_API std::unique_ptr<Scene> Main;
        static DLL_API Components::Camera *EditorSceneCamera;
        Components::Camera *main_camera_object = nullptr;
        entt::registry EntityRegistry;
        std::vector<std::shared_ptr<GameObject>> GameObjects;
        static DLL_API std::vector<std::shared_ptr<GameObject>> StaticGameObjects;
        std::string scene_file_path;

        void SetMainCameraObject(const std::shared_ptr<GameObject> &camObject);
        static DLL_API void MakeSceneCamera();
        void UpdateGameObjectComponents();
        static DLL_API void UpdateStaticGameObjectComponents();

        void DeleteStaticGameObject(std::shared_ptr<GameObject> &gameObject);
        void SetSceneCameraAsMain();

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine