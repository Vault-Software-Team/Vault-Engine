#include <Engine/GameObject.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Components/Transform.hpp>

namespace Engine {
    std::shared_ptr<GameObject> &GameObject::New(const std::string &name, const std::string &tag) {
        Scene::Main->GameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return Scene::Main->GameObjects.back();
    }

    GameObject::GameObject(const std::string &name, const std::string &tag) : name(name), tag(tag) {
        entity = Scene::Main->EntityRegistry.create();
        AddComponent<Components::Transform>();
    }
} // namespace Engine