#include <Engine/Scene.hpp>
#include <iostream>

namespace Engine {
    Scene::Scene(const std::string &scene_file) : scene_file_path(scene_file) {
    }

    void Scene::New(const std::string &scene_file) {
        Main = std::make_unique<Scene>(scene_file);
    }

    Scene::~Scene() {
        std::cout << "Deleting scene...\n";
        GameObjects.clear();
    }
} // namespace Engine