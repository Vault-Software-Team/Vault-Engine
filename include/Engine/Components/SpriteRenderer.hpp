#pragma once
#include <dllapi.hpp>
#include <entt/entt.hpp>
#include "Base.hpp"
#include <glm/ext.hpp>
#include <Renderer/Mesh.hpp>
#include "Transform.hpp"

namespace Engine {
    namespace Components {
        struct DLL_API SpriteRenderer : Base {
            static inline const std::string display_name = "Sprite Renderer";
            std::unique_ptr<VaultRenderer::Mesh> mesh;
            Transform *transform;
            std::unique_ptr<VaultRenderer::Shader> custom_shader;
            std::string custom_shader_path;
            SpriteRenderer() = default;
            void Init() override;

            void Draw(VaultRenderer::Shader &shader);
            void OnGUI() override;
        };
    } // namespace Components
} // namespace Engine