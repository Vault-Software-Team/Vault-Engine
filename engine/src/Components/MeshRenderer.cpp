#include <Engine/Components/MeshRenderer.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void MeshRenderer::SetMeshType(const MeshType &a_mesh_type) {
        mesh_type = a_mesh_type;
        mesh.reset();

        switch (mesh_type) {
        case MESH_PLANE: {
            std::vector<VaultRenderer::Vertex> vertices = {
                {glm::vec3(-0.5, 0, 0.5), glm::vec2(0, 0), glm::vec3(0, 1, 0)},
                {glm::vec3(-0.5, 0, -0.5), glm::vec2(0, 1), glm::vec3(0, 1, 0)},
                {glm::vec3(0.5, 0, -0.5), glm::vec2(1, 1), glm::vec3(0, 1, 0)},
                {glm::vec3(0.5, 0, 0.5), glm::vec2(1, 0), glm::vec3(0, 1, 0)} //
            };

            std::vector<uint32_t>
                indices = {0, 1, 2, 0, 2, 3};

            mesh = std::make_shared<VaultRenderer::Mesh>(vertices, indices);
            break;
        }
        case MESH_PYRAMID: {
            std::vector<VaultRenderer::Vertex> vertices = {
                // {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1, 1)},
                // {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1, 0)},
                // {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0, 0)},
                // {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0, 1)},

                // {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f)},
                // {glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(5.0f, 0.0f)},
                // {glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 0.0f)},
                // {glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(5.0f, 0.0f)},
                // {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f)}};

                {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},  // Bottom side
                {glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 5.0f), glm::vec3(0.0f, -1.0f, 0.0f)}, // Bottom side
                {glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(5.0f, 5.0f), glm::vec3(0.0f, -1.0f, 0.0f)},  // Bottom side
                {glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)},   // Bottom side

                {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(-0.8f, 0.5f, 0.0f)},  // Left Side
                {glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(5.0f, 0.0f), glm::vec3(-0.8f, 0.5f, 0.0f)}, // Left Side
                {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f), glm::vec3(-0.8f, 0.5f, 0.0f)},   // Left Side

                {glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(5.0f, 0.0f), glm::vec3(0.0f, 0.5f, -0.8f)},
                {glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.5f, -0.8f)}, // Non-facing side
                {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f), glm::vec3(0.0f, 0.5f, -0.8f)},  // Non-facing side

                {glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.8f, 0.5f, 0.0f)}, // Right side
                {glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(5.0f, 0.0f), glm::vec3(0.8f, 0.5f, 0.0f)},  // Right side
                {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f), glm::vec3(0.8f, 0.5f, 0.0f)},  // Right side

                {glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(5.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.8f)}, // Facing side
                {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.8f)},
                {glm::vec3(0.0f, 0.8f, 0.0f), glm::vec2(2.5f, 5.0f), glm::vec3(0.0f, 0.5f, 0.8f)},
            };

            std::vector<uint32_t> indices = {
                0, 1, 2,    // Bottom side
                0, 2, 3,    // Bottom side
                4, 6, 5,    // Left side
                7, 9, 8,    // Non-facing side
                10, 12, 11, // Right side
                13, 15, 14};
            mesh = std::make_shared<VaultRenderer::Mesh>(vertices, indices);
            break;
        }

        default: {
            break;
        }
        }
    }

    void MeshRenderer::SetCustomMeshType(std::vector<VaultRenderer::Vertex> &vertices, std::vector<uint32_t> &indices) {
        mesh_type = MESH_CUSTOM_MODEL;
        mesh.reset();

        mesh = std::make_shared<VaultRenderer::Mesh>(vertices, indices);
    }

    void MeshRenderer::OnGUI() {
        if (ImGui::TreeNode("Mesh Renderer")) {
            if (ImGui::Button("Select Mesh Type"))
                ImGui::OpenPopup("MeshRenderer_SelectMeshType");

            if (ImGui::BeginPopup("MeshRenderer_SelectMeshType")) {
                if (ImGui::Button("Plane", ImVec2(200, 0))) {
                    SetMeshType(MESH_PLANE);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Pyramid", ImVec2(200, 0))) {
                    SetMeshType(MESH_PYRAMID);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::Button("Cube", ImVec2(200, 0))) {
                    SetMeshType(MESH_CUBE);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            switch (mesh_type) {
            case MESH_PLANE:
                ImGui::Text("Type: Plane");
                break;
            case MESH_CUBE:
                ImGui::Text("Type: Cube");
                break;
            case MESH_PYRAMID:
                ImGui::Text("Type: Pyramid");
                break;
            case MESH_CUSTOM_MODEL:
                ImGui::Text("Type: Custom Model");
                break;
            default:
                break;
            }

            ImVec2 winSize = ImGui::GetWindowSize();
            if (ImGui::Button(ICON_FA_TRASH " Delete", ImVec2(winSize.x - 50, 0))) {
                GameObject::FindGameObjectByEntity(entity)->RemoveComponent<MeshRenderer>();
            }

            ImGui::TreePop();
        }
    }
} // namespace Engine::Components