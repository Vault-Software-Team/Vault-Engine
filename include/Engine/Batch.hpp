#pragma once
#include <dllapi.hpp>
#include <Renderer/Mesh.hpp>
#include <entt/entt.hpp>

namespace Engine {
    class DLL_API Batch {
    private:
        uint32_t VBO, VAO, EBO;
        int mesh_max_index = 0;

    public:
        std::vector<entt::entity> entities;
        std::vector<VaultRenderer::Vertex> vertices;
        std::vector<uint32_t> indices;

        Batch(entt::entity entity, std::unique_ptr<VaultRenderer::Mesh> &mesh);
        void AddMesh(entt::entity entity, std::unique_ptr<VaultRenderer::Mesh> &mesh);
        void Draw(VaultRenderer::Shader &shader);
    };
} // namespace Engine