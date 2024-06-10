#pragma once
#include <dllapi.hpp>
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include <Engine/Components/Camera.hpp>
#include <Box2D/Box2D.h>

namespace Engine {
    class GameObject;

    class PhysisContactListener : public b2ContactListener {
        void BeginContact(b2Contact *contact);
        void EndContact(b2Contact *contact);
    };

    class DLL_API Scene {
    public:
        static DLL_API std::vector<std::shared_ptr<GameObject>> StaticGameObjects;
        static DLL_API entt::registry StaticGameObjects_EntityRegistry;
        static DLL_API Components::Camera *EditorSceneCamera;
        static DLL_API std::shared_ptr<Scene> Main;
        static DLL_API std::shared_ptr<Scene> EditorScene;

        static DLL_API std::shared_ptr<Scene> New(const std::string &scene_file);
        static DLL_API void SetMainScene(std::shared_ptr<Scene> &scene);
        static DLL_API void SetEditorScene(std::shared_ptr<Scene> &scene);
        static DLL_API std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> &other);
        static DLL_API std::shared_ptr<Scene> CopyAndSetAsMain(std::shared_ptr<Scene> &other);

        std::shared_ptr<GameObject> &FindGameObjectByEntity(const entt::entity &entity);
        std::shared_ptr<GameObject> &FindGameObjectByID(const std::string &id);
        std::shared_ptr<GameObject> &FindGameObjectByName(const std::string &name);
        std::shared_ptr<GameObject> &FindGameObjectByTag(const std::string &tag);
        std::shared_ptr<GameObject> &MakeGameObject(const std::string &name, const std::string &tag);

        std::string scene_file_path;
        Components::Camera *main_camera_object = nullptr;
        std::vector<std::shared_ptr<GameObject>> GameObjects;
        entt::registry EntityRegistry;
        std::unique_ptr<b2World> Physics2DWorld;
        std::unique_ptr<PhysisContactListener> b2_contact_listener;

        void
        SetMainCameraObject(const std::shared_ptr<GameObject> &camObject, bool static_registry = false);
        static DLL_API void MakeSceneCamera();
        void UpdateGameObjectComponents();
        static DLL_API void UpdateStaticGameObjectComponents();

        void DeleteStaticGameObject(std::shared_ptr<GameObject> &gameObject);
        void SetSceneCameraAsMain();

        // Events
        void OnRuntimeStart();
        void OnRuntimeStop();
        void OnRuntimeUpdate(const float ts);

        // Physics 2D Calls
        void Setup2DPhysicsWorld();
        void Step2DPhysicsWorld(const float ts);

        // Scripting Calls
        void SetupHyperScript();
        void UpdateHyperScript();
        void SetupCSharp();
        void SetupCSharpOnStart();
        void UpdateCSharp();

        Scene(const std::string &scene_file);
        ~Scene();
    };
} // namespace Engine