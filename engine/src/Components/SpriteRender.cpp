#include <Engine/Components/SpriteRenderer.hpp>
#include <Engine/Scene.hpp>
#include <iostream>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>
#include <imgui/imgui_stdlib.h>

using namespace VaultRenderer;
namespace Engine::Components {
    void SpriteRenderer::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);

        std::vector<Vertex> vertices = {
            Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0, 1, 0)},
            Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0, 1, 0)} //
        };
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        mesh = std::make_unique<Mesh>(vertices, indices);
    }

    void SpriteRenderer::Draw(VaultRenderer::Shader &shader) {
        if (mesh)
            mesh->Draw(shader);
    }

    void SpriteRenderer::OnGUI() {
        if (ImGui::TreeNode(display_name.c_str())) {
            ImGui::ColorEdit4("Color", &mesh->material.color.x);

            ImGui::Text("Sprite");
            ImGui::ImageButton((mesh->material.diffuse ? (void *)mesh->material.diffuse->texture_data->ID : 0), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemClicked(1)) {
                mesh->material.diffuse.reset();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("image_file")) {
                    std::string path = (char *)payload->Data;
                    mesh->material.SetDiffuse(path);
                }
            }

            ImVec2 winSize = ImGui::GetWindowSize();
            if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x - 50, 0))) {
                GameObject::FindGameObjectByEntity(entity)->RemoveComponent<SpriteRenderer>();
            }

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components