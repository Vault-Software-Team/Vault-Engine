#include "Engine/Components/ModelAnimator.hpp"
#include "Engine/SceneSerialization.hpp"
#include <Editor/GUI/MainGUI.hpp>
#include <Engine/Scene.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/Components/IncludeComponents.hpp>
#include <icons/icons.h>
#include <imgui/imgui.h>

using namespace Engine;
using namespace Engine::Components;

namespace Editor {
    void GUI::Hierarchy() {
        constexpr int search_size = 256;
        static char search[search_size];

        std::string filterStr = search;
        std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), asciitolower);

        ImGui::Begin(ICON_FA_CUBES " Hierarchy");
        if (ImGui::Button(ICON_FA_PLUS " Add GameObject", ImVec2(ImGui::GetWindowSize().x - 20, 25))) {
            auto &gameObject = GameObject::New("GameObject");
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();

        ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20);
        ImGui::InputText("##HierarhcySearch", search, search_size);

        ImVec2 cursor_pos = ImGui::GetCursorPos();
        ImGui::Dummy(ImGui::GetContentRegionAvail());
        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
            GUI::selected_gameObject = nullptr;
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("prefab")) {
                Serializer::LoadPrefab((char *)payload->Data);
            }
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("gameobject")) {
                for (auto &gameObject : Scene::Main->GameObjects) {
                    if (gameObject->ID == (char *)payload->Data) {
                        gameObject->parent = "NO_PARENT";
                        break;
                    }
                }
            }
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("model_file")) {
                Engine::ModelMesh *model = new Engine::ModelMesh((char *)payload->Data);

                auto &gameObject = GameObject::New("Model");

                int i = 0;
                for (auto &mesh : model->meshes) {
                    auto &_gameObject = GameObject::New(mesh.name);
                    _gameObject->AddComponent<MeshRenderer>();
                    _gameObject->parent = gameObject->ID;

                    auto &meshRenderer = _gameObject->GetComponent<MeshRenderer>();
                    meshRenderer.SetCustomMeshType(mesh.vertices, mesh.indices);
                    meshRenderer.mesh_index = i;
                    meshRenderer.mesh_path = model->path;
                    meshRenderer.model = std::shared_ptr<ModelMesh>(model);

                    i++;
                }
                gameObject->AddComponent<ModelAnimator>();
                auto &comp = gameObject->AddComponent<ModelAnimator>();
                comp.model = std::make_unique<ModelMesh>((char *)payload->Data);
            }

            ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(cursor_pos);

        for (auto &gameObject : Scene::Main->GameObjects) {
            if (!gameObject)
                continue;

            if (gameObject->parent != "NO_PARENT")
                continue;

            std::string n = gameObject->name;
            std::transform(n.begin(), n.end(), n.begin(), asciitolower);
            if (n.find(filterStr) == std::string::npos && filterStr != "")
                continue;

            gameObject->GUI();
        }

        ImGui::End();
    }
} // namespace Editor