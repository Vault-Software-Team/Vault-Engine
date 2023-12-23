#include <Engine/Components/MeshRenderer.hpp>

namespace Engine::Components {
    void MeshRenderer::SetMeshType(const MeshType &a_mesh_type) {
        mesh_type = a_mesh_type;
        mesh.reset();

        switch (mesh_type) {
        case MESH_PLANE: {
            std::vector<VaultRenderer::Vertex> vertices = {
                {glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1, 1)},
                {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1, 0)},
                {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0, 0)},
                {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0, 1)}};

            std::vector<uint32_t> indices = {
                0, 1, 3,
                1, 2, 3};

            mesh = std::make_unique<VaultRenderer::Mesh>(vertices, indices);
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
            mesh = std::make_unique<VaultRenderer::Mesh>(vertices, indices);
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

        mesh = std::make_unique<VaultRenderer::Mesh>(vertices, indices);
    }
} // namespace Engine::Components