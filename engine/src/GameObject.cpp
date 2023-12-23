#include <Engine/GameObject.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <uuid.hpp>
#include <iostream>

namespace Engine {
    std::shared_ptr<GameObject> &GameObject::New(const std::string &name, const std::string &tag) {
        Scene::Main->GameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return Scene::Main->GameObjects.back();
    }

    GameObject::GameObject(const std::string &name, const std::string &tag) : name(name), tag(tag) {
        entity = Scene::Main->EntityRegistry.create();
        AddComponent<Components::Transform>();
        ID = uuid::generate_uuid_v4();
    }

    void GameObject::UpdateComponents(VaultRenderer::Shader &shader) {
        using namespace Components;
        if (HasComponent<MeshRenderer>()) {
            auto &meshRenderer = GetComponent<MeshRenderer>();
            if (meshRenderer.mesh) {
                if (meshRenderer.mesh_type == Engine::Components::MESH_PLANE) {
                    glDisable(GL_CULL_FACE);
                } else {
                    glEnable(GL_CULL_FACE);
                }

                GetComponent<Transform>().Update();
                shader.SetUniformMat4("transformModel", GetComponent<Transform>().model);

                meshRenderer.mesh->Draw(shader);
            }
        }

        if (HasComponent<AmbientLight>()) {
            auto &light = GetComponent<AmbientLight>();
            light.AttachToShader(shader);
        }

        if (HasComponent<PointLight>()) {
            auto &light = GetComponent<PointLight>();
            light.AttachToShader(shader);
        }

        if (HasComponent<DirectionalLight>()) {
            auto &light = GetComponent<DirectionalLight>();
            light.AttachToShader(shader);
        }

        if (HasComponent<SpotLight>()) {
            auto &light = GetComponent<SpotLight>();
            light.AttachToShader(shader);
        }

        for (auto &pChild : Scene::Main->GameObjects) {
            if (pChild->parent == ID && pChild->ID != ID)
                pChild->UpdateComponents(shader);
        }
    }

    void GameObject::SetParent(std::shared_ptr<GameObject> &parent) {
        this->parent = parent->ID;
    }

    std::shared_ptr<GameObject> &GameObject::AddChild(const std::string &name, const std::string &tag) {
        auto &pChild = GameObject::New(name, tag);
        pChild->parent = ID;
        return pChild;
    }

    std::shared_ptr<GameObject> &GameObject::AddChild(std::shared_ptr<GameObject> &child) {
        child->parent = ID;
        return child;
    }

    std::shared_ptr<GameObject> &GameObject::FindGameObjectByEntity(const entt::entity &entity) {
        for (auto &pGameObject : Scene::Main->GameObjects) {
            if (pGameObject->entity == entity)
                return pGameObject;
        }
    }
    std::shared_ptr<GameObject> &GameObject::FindGameObjectByID(const std::string &ID) {
        for (auto &pGameObject : Scene::Main->GameObjects) {
            if (pGameObject->ID == ID)
                return pGameObject;
        }
    }

    std::shared_ptr<GameObject> &GameObject::FindGameObjectByName(const std::string &name) {
        for (auto &pGameObject : Scene::Main->GameObjects) {
            if (pGameObject->name == name)
                return pGameObject;
        }
    }

} // namespace Engine