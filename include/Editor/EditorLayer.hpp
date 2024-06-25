#pragma once
#include "Renderer/Mesh.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include <dllapi.hpp>
#include <memory>

namespace Editor {
    class DLL_API EditorLayer {
    public:
        static DLL_API EditorLayer *instance;

        bool EnableIconGizmo = true;
        bool EnableColliderGizmo = true;

        std::unique_ptr<VaultRenderer::Mesh> IconMesh = nullptr;
        std::unique_ptr<VaultRenderer::Shader> IconShader = nullptr;

        struct Icons {
            std::unique_ptr<VaultRenderer::Texture> CameraIcon;
            std::unique_ptr<VaultRenderer::Texture> DirLightIcon;
            std::unique_ptr<VaultRenderer::Texture> PointLightIcon;
            std::unique_ptr<VaultRenderer::Texture> SpotLightIcon;
            std::unique_ptr<VaultRenderer::Texture> AudioIcon;
        } icons;

        struct IconMeshes {
            std::unique_ptr<VaultRenderer::Mesh> CameraIcon;
            std::unique_ptr<VaultRenderer::Mesh> DirLightIcon;
            std::unique_ptr<VaultRenderer::Mesh> PointLightIcon;
            std::unique_ptr<VaultRenderer::Mesh> SpotLightIcon;
            std::unique_ptr<VaultRenderer::Mesh> AudioIcon;
        } iconMeshes;

        struct s_ColliderGizmo {
            std::unique_ptr<VaultRenderer::Mesh> Gizmo3D;
            VaultRenderer::Shader *shader;
            void DrawGizmo(VaultRenderer::Shader &shader, VaultRenderer::Mesh *mesh, glm::mat4 &model, glm::vec3 colliderScale);
        } ColliderGizmo;

        EditorLayer();
        void GUI();
        void GameGUI();

        // make sure the shader is *EditorLayer::instance->IconShader !!
        void DrawIcon(std::unique_ptr<VaultRenderer::Mesh> &icon_mesh, VaultRenderer::Shader &shader, const glm::mat4 &transform, uint32_t entity, std::unique_ptr<VaultRenderer::Texture> &texture);
    };
} // namespace Editor