#include <Engine/Scene.hpp>
#include <iostream>
#include <Engine/GameObject.hpp>
#include <Engine/Runtime.hpp>
#include <Editor/GUI/MainGUI.hpp>

namespace Engine {
    std::unique_ptr<Scene> Scene::Main;
    Components::Camera *Scene::EditorSceneCamera = nullptr;
    std::vector<std::shared_ptr<GameObject>> Scene::StaticGameObjects;

    Scene::Scene(const std::string &scene_file) : scene_file_path(scene_file) {
    }

    void Scene::New(const std::string &scene_file) {
        Main = std::make_unique<Scene>(scene_file);
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

} // namespace Engine