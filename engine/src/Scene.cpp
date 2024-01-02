#include <Engine/Scene.hpp>
#include <iostream>
#include <Engine/GameObject.hpp>
#include <Engine/Runtime.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Components/IncludeComponents.hpp>

namespace Engine {
    DLL_API std::shared_ptr<Scene> Scene::Main;
    DLL_API std::shared_ptr<Scene> Scene::EditorScene;

    Components::Camera *Scene::EditorSceneCamera = nullptr;
    std::vector<std::shared_ptr<GameObject>> Scene::StaticGameObjects;

    Scene::Scene(const std::string &scene_file) : scene_file_path(scene_file) {}

    std::shared_ptr<Scene> Scene::New(const std::string &scene_file) {
        return std::move(std::make_shared<Scene>(scene_file));
    }

    void Scene::SetMainScene(std::shared_ptr<Scene> &scene) {
        Main = scene;
    }

    void Scene::SetEditorScene(std::shared_ptr<Scene> &scene) {
        EditorScene = scene;
    }

    std::shared_ptr<GameObject> &Scene::MakeGameObject(const std::string &name, const std::string &tag) {
        GameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return GameObjects.back();
    }

    Scene::~Scene() {
        Editor::GUI::selected_gameObject = nullptr;
        std::cout << "Deleting scene...\n";
        GameObjects.clear();
        main_camera_object = nullptr;
    }

    void Scene::SetMainCameraObject(const std::shared_ptr<GameObject> &camObject) {
        main_camera_object = &camObject->GetComponent<Components::Camera>();
    }

    void Scene::UpdateGameObjectComponents() {
        for (auto gameObject : GameObjects) {
            gameObject->UpdateComponents(*Runtime::default_shader);
        }
    }

    void Scene::UpdateStaticGameObjectComponents() {
        for (auto gameObject : StaticGameObjects) {
            gameObject->UpdateComponents(*Runtime::default_shader);
        }
    }

    void Scene::DeleteStaticGameObject(std::shared_ptr<GameObject> &gameObject) {
        for (int i = 0; i < StaticGameObjects.size(); i++) {
            if (StaticGameObjects[i]->entity == gameObject->entity) {
                StaticGameObjects.erase(StaticGameObjects.begin() + i);
                break;
            }
        }
    }

    void Scene::MakeSceneCamera() {
        StaticGameObjects.push_back(std::make_shared<GameObject>("VAULT_SceneCamera", "VAULT_SceneCamera"));
        auto &cam = StaticGameObjects.back();
        cam->AddComponent<Components::Camera>();
        cam->GetComponent<Components::Transform>().rotation.z = -1.0;

        cam->GetComponent<Components::Camera>().fov = 45;
        cam->GetComponent<Components::Camera>().near = 0.01f;
        cam->GetComponent<Components::Camera>().far = 100.0f;

        cam->GetComponent<Components::Camera>().width = 800;
        cam->GetComponent<Components::Camera>().height = 600;
        cam->GetComponent<Components::Transform>().position.y = 0.5f;
        cam->GetComponent<Components::Transform>().position.z = 25.f;

        EditorSceneCamera = &cam->GetComponent<Components::Camera>();
    }

    void Scene::SetSceneCameraAsMain() {
        main_camera_object = EditorSceneCamera;
    }

    std::shared_ptr<GameObject> &Scene::FindGameObjectByEntity(const entt::entity &entity) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->entity == entity)
                return pGameObject;
        }
    }
    std::shared_ptr<GameObject> &Scene::FindGameObjectByID(const std::string &ID) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->ID == ID)
                return pGameObject;
        }
    }

    std::shared_ptr<GameObject> &Scene::FindGameObjectByName(const std::string &name) {
        for (auto &pGameObject : GameObjects) {
            if (pGameObject->name == name)
                return pGameObject;
        }
    }

    template <typename T>
    static void CopyComponent(entt::registry &dest, entt::registry &src) {
        using namespace Components;
        auto src_view = src.view<T>();

        for (auto se : src_view) {
            auto &component = src.get<T>(se);
            auto dest_view = dest.view<Transform>();
            for (auto de : dest_view) {
                if (dest_view.get<Transform>(de).ID == src.get<Transform>(de).ID) {
                    dest.emplace_or_replace<T>(de, component);
                }
            }
        }
    }

    std::shared_ptr<Scene> Scene::Copy(std::shared_ptr<Scene> &other) {
        using namespace Components;
        std::shared_ptr<Scene> new_scene = Scene::New("");
        // Scene::SetEditorScene(other);
        // Scene::SetMainScene(new_scene);

        auto &source_reg = other->EntityRegistry;
        auto &destination_reg = new_scene->EntityRegistry;

        auto view = source_reg.view<Transform>();
        for (auto e : view) {
            auto &gameObject = other->FindGameObjectByEntity(e);
            auto &new_gameObject = new_scene->MakeGameObject(gameObject->name, gameObject->tag);
            new_gameObject->ID = gameObject->ID;
            new_gameObject->parent = gameObject->parent;

            auto &transform = destination_reg.get<Transform>(e);
            transform.ID = new_gameObject->ID;
        }

        CopyComponent<Transform>(destination_reg, source_reg);
        CopyComponent<Camera>(destination_reg, source_reg);
        CopyComponent<MeshRenderer>(destination_reg, source_reg);
        CopyComponent<AmbientLight>(destination_reg, source_reg);
        CopyComponent<PointLight>(destination_reg, source_reg);
        CopyComponent<DirectionalLight>(destination_reg, source_reg);
        CopyComponent<SpotLight>(destination_reg, source_reg);
        CopyComponent<Text3D>(destination_reg, source_reg);

        return new_scene;
    }
} // namespace Engine