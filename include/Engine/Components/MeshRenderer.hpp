#pragma once
#include <dllapi.hpp>
#include <glm/ext.hpp>
#include "Base.hpp"
#include <Renderer/Mesh.hpp>
#include <memory>
#include <Renderer/Texture.hpp>

namespace Engine {
    namespace Components {
        enum MeshType {
            MESH_PLANE,
            MESH_PYRAMID,
            MESH_CUBE,
            MESH_CUSTOM_MODEL,
            MESH_NONE
        };

        struct DLL_API MeshRenderer : Base {
            MeshType mesh_type = MESH_NONE;
            std::unique_ptr<VaultRenderer::Mesh> mesh = nullptr;

            MeshRenderer() = default;

            void SetMeshType(const MeshType &a_mesh_type);
            void SetCustomMeshType(std::vector<VaultRenderer::Vertex> &vertices, std::vector<uint32_t> &indices);
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine