#include "Editor/EditorLayer.hpp"
#include "Engine/Components/BoxCollider2D.hpp"
#include "Engine/Components/Collider3D.hpp"
#include "Engine/Components/MeshRenderer.hpp"
#include "Engine/Components/ModelAnimator.hpp"
#include "Engine/Components/Rigidbody3D.hpp"
#include "Engine/Components/Transform.hpp"
#include "Engine/Discord.hpp"
#include "Engine/Runtime.hpp"
#include "Renderer/Shader.hpp"
#include "StupidFunctions.hpp"
#include <Engine/GameObject.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <algorithm>
#include <uuid.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <imgui/imgui.h>
#include <icons/icons.h>

namespace Engine {
    DLL_API std::vector<std::shared_ptr<GameObject>> GameObject::scheduled_deletions;

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

    GameObject::GameObject(entt::registry &registry, const std::string &name, const std::string &tag) {
        entity = registry.create();
        AddComponent<Components::Transform>(registry);
        ID = uuid::generate_uuid_v4();
        another_registry = true;
        other_registry = &registry;
    }

    GameObject::~GameObject() {
        if (another_registry && other_registry) {
            other_registry->destroy(entity);
        } else {
            Scene::Main->EntityRegistry.destroy(entity);
        }
    }

    void GameObject::UpdateRendering(VaultRenderer::Shader &shader, bool update_children) {
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
                shader.Bind();
                shader.SetUniformMat4("transformModel", GetComponent<Transform>().model);
                // Runtime::instance->c_ShadowMap->BindToShader(shader);

                // meshRenderer.AnimateAndSetUniforms(shader);
                if (meshRenderer.mesh_type == MESH_PLANE) shader.SetUniform1i("mesh_isFlat", true);
                shader.SetUniform1ui("u_EntityID", (uint32_t)entity);

                auto &thisGo = GameObject::FindGameObjectByEntity(meshRenderer.entity);
                if (thisGo->parent != "NO_PARENT") {
                    auto &parent = GameObject::FindGameObjectByID(thisGo->parent);
                    if (parent->HasComponent<ModelAnimator>()) {
                        auto &comp = parent->GetComponent<ModelAnimator>();
                        comp.BindToShader(shader);
                    }
                }

                meshRenderer.mesh->Draw(shader);
            }
        }

        if (HasComponent<ModelRenderer>()) {
            auto &modelRenderer = GetComponent<ModelRenderer>();
            if (modelRenderer.model) {
                glEnable(GL_CULL_FACE);
                auto &transform = GetComponent<Transform>();

                transform.Update();
                shader.Bind();
                // shader.SetUniformMat4("transformModel", transform.model);
                // Runtime::instance->c_ShadowMap->BindToShader(shader);
                shader.SetUniform1ui("u_EntityID", (uint32_t)entity);

                modelRenderer.AnimateAndSetUniforms(shader);
                modelRenderer.Draw(shader, transform.model);
            }
        }

        if (HasComponent<SpriteRenderer>()) {
            auto &spriteRenderer = GetComponent<SpriteRenderer>();
            if (spriteRenderer.mesh) {
                auto &transform = GetComponent<Transform>();
                glDisable(GL_CULL_FACE);
                transform.Update();
                shader.Bind();
                shader.SetUniformMat4("transformModel", transform.model);
                // Runtime::instance->c_ShadowMap->BindToShader(shader);
                shader.SetUniform1ui("u_EntityID", (uint32_t)entity);

                shader.SetUniform1i("mesh_isFlat", true);
                spriteRenderer.Draw(shader);
            }
        }

        if (HasComponent<SpritesheetRenderer>()) {
            auto &spritesheetRenderer = GetComponent<SpritesheetRenderer>();
            spritesheetRenderer.Update();
            if (spritesheetRenderer.mesh) {
                auto &transform = GetComponent<Transform>();
                glDisable(GL_CULL_FACE);
                transform.Update();
                shader.Bind();
                shader.SetUniformMat4("transformModel", transform.model);
                // Runtime::instance->c_ShadowMap->BindToShader(shader);
                shader.SetUniform1ui("u_EntityID", (uint32_t)entity);
                shader.SetUniform1i("u_UseTextureAlpha", 1);
                spritesheetRenderer.Draw(shader);
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

        if (!update_children) return;

        for (auto &pChild : Scene::Main->GameObjects) {
            if (pChild->parent == ID && pChild->ID != ID) pChild->UpdateRendering(shader);
        }
    }

    void GameObject::UpdateComponents(VaultRenderer::Shader &shader) {
        using namespace Components;

        UpdateRendering(shader);

        if (HasComponent<ModelAnimator>()) {
            auto &animator = GetComponent<ModelAnimator>();
            animator.PlayAnimation();
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

        if (HasComponent<Rigidbody3D>()) {
            auto &rb = GetComponent<Rigidbody3D>();
            rb.Update();
        }

        for (auto &pChild : Scene::Main->GameObjects) {
            if (pChild->parent == ID && pChild->ID != ID) pChild->UpdateComponents(shader);
        }
    }

    void GameObject::SetParent(std::shared_ptr<GameObject> &parent) {
        this->parent = parent->ID;
    }

    std::shared_ptr<GameObject> &GameObject::AddChild(const std::string &name, const std::string &tag) {
        auto &pChild = GameObject::New(name, tag);
        pChild->parent = ID;
        std::cout << ID << " " << pChild->parent << " " << pChild->name << " " << this->name << "\n";
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

    std::shared_ptr<GameObject> &GameObject::FindGameObjectByTag(const std::string &tag) {
        for (auto &pGameObject : Scene::Main->GameObjects) {
            if (pGameObject->tag == tag)
                return pGameObject;
        }
    }

    void GameObject::GUI() {
        int index = -1;
        auto it = std::find(Scene::Main->GameObjects.begin(), Scene::Main->GameObjects.end(), shared_from_this());

        if (it != Scene::Main->GameObjects.end()) {
            index = it - Scene::Main->GameObjects.begin();
        }

        if (index == -1)
            return;

        bool hasChildren = false;
        for (auto &gameObject : Scene::Main->GameObjects) {
            if (gameObject->parent == ID) {
                hasChildren = true;
                break;
            }
        }
        ImVec4 color{255, 255, 255, 1};
        Editor::GUI::SetNameIcon(icon, color, this);
        color.x /= 255;
        color.y /= 255;
        color.z /= 255;

        // ImGui::TextColored(color, icon.c_str());
        // ImGui::SameLine();

        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 5));

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("gameobject")) {
                for (auto &gameObject : Scene::Main->GameObjects) {
                    if (gameObject->ID == (char *)payload->Data) {
                        int gIndex = -1;
                        auto it = find(Scene::Main->GameObjects.begin(), Scene::Main->GameObjects.end(), gameObject);

                        if (it != Scene::Main->GameObjects.end()) {
                            gIndex = it - Scene::Main->GameObjects.begin();
                        }

                        if (gIndex == -1)
                            continue;

                        // std::swap(Scene::Main->GameObjects[gIndex], Scene::Main->GameObjects[index]);
                        vector_move(Scene::Main->GameObjects, gIndex, index);
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }
        if (hasChildren) {
            bool tree_node_open = ImGui::TreeNodeEx(ID.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth, "%s", (icon + " " + name).c_str());
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload =
                        ImGui::AcceptDragDropPayload("gameobject")) {
                    if ((char *)payload->Data != ID) {
                        for (auto &gameObject : Scene::Main->GameObjects) {
                            if (gameObject->ID == (char *)payload->Data) {
                                gameObject->parent = ID;
                                break;
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                Editor::GUI::dragPayload = ID.c_str();
                ImGui::SetDragDropPayload("gameobject", ID.c_str(), ID.length() + 1);
                ImGui::Text("%s %s", ICON_FA_CUBE, name.c_str());
                ImGui::EndDragDropSource();
            }
            GUI_ContextMenu();
            if (tree_node_open) {
                for (auto &gameObject : Scene::Main->GameObjects) {
                    if (gameObject->parent == ID) {
                        gameObject->GUI();
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && !ImGui::IsMouseDragging(0)) {
                DiscordRPC::instance->UpdatePresence("In Editor", "Inspecting " + name);
                Editor::GUI::selected_gameObject = this;
            }
        } else {
            ImGui::PushID(ID.c_str());
            if (ImGui::Selectable((icon + " " + name).c_str(), false, ImGuiSelectableFlags_SpanAvailWidth)) {
                DiscordRPC::instance->UpdatePresence("In Editor", "Inspecting " + name);
                Editor::GUI::selected_gameObject = this;
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload =
                        ImGui::AcceptDragDropPayload("gameobject")) {
                    if ((char *)payload->Data != ID) {
                        for (auto &gameObject : Scene::Main->GameObjects) {
                            if (gameObject->ID == (char *)payload->Data) {
                                gameObject->parent = ID;
                                break;
                            }
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                Editor::GUI::dragPayload = ID.c_str();
                ImGui::SetDragDropPayload("gameobject", ID.c_str(), ID.length() + 1);
                ImGui::Text("%s %s", ICON_FA_CUBE, name.c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::PopID();
            GUI_ContextMenu();
        }
        ImGui::PopStyleColor();

        if (ImGui::BeginPopup(("GameObject_" + ID).c_str())) {
            ImGui::Text("%s %s", icon.c_str(), name.c_str());
            if (ImGui::Button("Select GameObject", ImVec2(200, 0))) {
                Editor::GUI::selected_gameObject = this;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::Button(ICON_FA_TRASH " Delete GameObject", ImVec2(200, 0))) {
                scheduled_deletions.push_back(FindGameObjectByEntity(entity));
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    void GameObject::GUI_ContextMenu() {
        if (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            scheduled_deletions.push_back(FindGameObjectByEntity(entity));
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
            ImGui::OpenPopup(("GameObject_" + ID).c_str());
        }
    }

    void GameObject::UNSAFE_DeleteGameObject() {
        if (Editor::GUI::selected_gameObject) {
            if (Editor::GUI::selected_gameObject->ID == ID)
                Editor::GUI::selected_gameObject = nullptr;
        }
        // Get the shared pointer of this instance
        auto &sharedPtr = FindGameObjectByEntity(entity);
        std::vector<std::shared_ptr<GameObject>> children_to_delete;
        for (std::vector<std::shared_ptr<GameObject>>::iterator it = Scene::Main->GameObjects.begin(); it != Scene::Main->GameObjects.end();) {
            if (it->get()->parent == ID) {
                it->get()->parent = "NO_PARENT";
                it->get()->UNSAFE_DeleteGameObject();
            } else
                ++it;
        }
        // Destroy the entity from the EnTT Registry
        // Remove the GameObject from GameObjects
        Scene::Main->GameObjects.erase(std::remove(Scene::Main->GameObjects.begin(), Scene::Main->GameObjects.end(), sharedPtr), Scene::Main->GameObjects.end());
    }
} // namespace Engine