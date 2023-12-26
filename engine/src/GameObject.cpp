#include <Engine/GameObject.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <uuid.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <imgui/imgui.h>

namespace Engine {
    std::shared_ptr<GameObject> &GameObject::New(const std::string &name, const std::string &tag) {
        Scene::Main->GameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return Scene::Main->GameObjects.back();
    }

    std::shared_ptr<GameObject> &GameObject::NewStatic(const std::string &name, const std::string &tag) {
        Scene::StaticGameObjects.push_back(std::make_shared<GameObject>(name, tag));
        return Scene::StaticGameObjects.back();
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

        if (HasComponent<Text3D>()) {
            glDisable(GL_CULL_FACE);
            auto &text3D = GetComponent<Text3D>();
            if (VaultRenderer::Font::font_shader != nullptr) {
                text3D.Draw(*VaultRenderer::Font::font_shader);
            }
            glEnable(GL_CULL_FACE);
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

    void GameObject::GUI() {
        std::string icon;
        bool hasChildren = false;
        for (auto &gameObject : Scene::Main->GameObjects) {
            if (gameObject->parent == gameObject->ID) {
                hasChildren = true;
                break;
            }
        }
        Editor::GUI::SetNameIcon(icon, this);

        if (hasChildren) {
            ImGui::TreeNode((icon + " " + name).c_str());
            for (auto &gameObject : Scene::Main->GameObjects) {
                if (gameObject->parent == gameObject->ID) {
                    gameObject->GUI();
                }
            }
            ImGui::TreePop();
        } else {
            ImGui::Selectable((icon + " " + name).c_str());
        }
    }
} // namespace Engine